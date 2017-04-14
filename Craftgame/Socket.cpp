#include "Socket.h"
#include <string>
#include <sstream>
#include <vector>
#include "Parsing.h"

Socket::Socket(std::string addr, unsigned short port) {
#ifdef WINDOWS
	w = true;
#endif
	timeout = 10;
#ifdef WINDOWS
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		throw CraftgameException((std::string("Failed. Error code: ") + std::to_string(WSAGetLastError())).c_str());
	}
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
                throw CraftgameException((std::string("Failed to create socket. Error code: ") + std::to_string(WSAGetLastError())).c_str());
    }
#else
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		throw CraftgameException("Failed to create socket.");
	}
#endif
#ifdef WINDOWS
	PADDRINFOA addrinfo;
	getaddrinfo(addr.c_str(), nullptr, nullptr, &addrinfo);
	if (!addrinfo)
		throw std::exception((std::string("Unresolved hostname: ") + addr).c_str());
	char* c = addrinfo->ai_addr->sa_data;
	server.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)(c[2]);
	server.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)(c[3]);
	server.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)(c[4]);
	server.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)(c[5]);
#else
	hostent* addrinfo = gethostbyname(addr.c_str());
	if (!addrinfo)
		throw  CraftgameException((std::string("Unresolved hostname: ") + addr).c_str());
	memset((char*)&server, 0, sizeof(server));
	memcpy((char*)addrinfo->h_addr, (char*)&server.sin_addr.s_addr, addrinfo->h_length);
	delete addrinfo;
#endif
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
#ifdef WINDOWS
	if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		throw  CraftgameException((std::string("Connect error. Error code: ") + std::to_string(WSAGetLastError())).c_str());
	}
#else
	if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
        {
                throw  CraftgameException("Connect error.");
        }
#endif
}
Socket::Socket(
#ifdef WINDOWS
SOCKET
#else
int
#endif

 socket) :
	s(socket) {
#ifdef WINDOWS
	w = false;
#endif
	timeout = 10;
}
void Socket::setTimeout(long _timeout) {
	timeout = _timeout;
}
void Socket::writeString(std::string message) {
	if (send(s, message.c_str(), strlen(message.c_str()), 0) < 0)
	{
		throw  CraftgameException("Send error");
	}
}

size_t Socket::readBytes(char* buf, size_t len, int flags) {
	size_t bytesRead = 0;
	fd_set fds;
	timeval tv;
#ifdef WINDOWS
	FD_ZERO(&fds, sizeof(fds));
	ZeroMemory(&tv, sizeof(tv));
	FD_SET(s, &fds);
	tv.tv_sec = timeout;
	int n = select(s, &fds, 0, 0, &tv);
#else
	FD_ZERO(&fds);
	memset(&tv, 0, sizeof(tv));
	FD_SET(s, &fds);
	tv.tv_sec = timeout;
	int n = select(s + 1, &fds, 0, 0, &tv);
#endif
	if (n == 0) {
		throw  CraftgameException("recv timeout");
	}
	else if (n < 0) {
		throw  CraftgameException("Set timeout error");
	}

	while (bytesRead < len) {
		int c = recv(s, buf, len, flags);
		if (c < 0) {
			throw  CraftgameException("An error has occurred while reading from socket");
		}
		bytesRead += c;
	}
	return 0;
}
void Socket::close() {
#ifdef WINDOWS
	closesocket(s);
	if (w)
		WSACleanup();
#else
	shutdown(s, 2);
#endif
}

void Socket::writeBytes(const char* c, size_t size) {
	if (send(s, c, size, 0) < 0)
	{
		throw  CraftgameException("Send error");
	}
}
/* */
#include "Exceptions.h"
void http_get(string address, ostream& o) {
	std::string spl = Parsing::splitString(address, "/")[0];
	Socket s(spl);
	s.writeString("GET ");
	s.writeString(address.substr(spl.size()));
	s.writeString(" HTTP/1.1\r\n");
	s.writeString("Host: " + spl + "\r\n");
	s.writeString("Connection: keep-alive\r\n");
	s.writeString("Accept-Charset: utf-8\r\n");
	s.writeString("Accept: */*\r\n\r\n");

	stringstream ss;
	size_t contentLength = 0;
	while (1) {
		char* c = new char;
		size_t bytes_read = s.readBytes(c, 1, 0);
		ss.write(c, 1);
		delete c;
		if (!ss.str().empty() && ss.str().find("\r\n\r\n") != std::string::npos) {
			string line;
			while (Parsing::getLine(ss, line, "\r\n")) {
				if (line.find("HTTP") == 0) {
					vector<string> s = Parsing::splitString(line, " ");
					if (s.size() < 3)
						throw  CraftgameException("Incorrect HTTP head");
					if (s[1] != "200")
						throw HttpException((string("HTTP status ") + line + " " + address).c_str(), stoi(s[1]));
				}
				else if (line.find("Content-Length:") == 0) {
					vector<string> s = Parsing::splitString(line, " ");
					if (s.size() != 2)
						throw  CraftgameException("Incorrect HTTP Content-Length");
					stringstream ss(s[1]);
					ss >> contentLength;
				}
			}
			break;
		}
	}
	s.read(o, [&](size_t bytes) {
		return bytes < contentLength;
	});
	s.close();
}
ServerSocket::ServerSocket(unsigned short port) {
#ifdef WINDOWS
	addrinfo xyi;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		throw  CraftgameException((std::string("Failed. Error code: ") + std::to_string(WSAGetLastError())).c_str());
	}
#else
	sockaddr_in xyi;
#endif
#ifdef WINDOWS
	ZeroMemory(&xyi, sizeof(xyi));
	xyi.ai_family = AF_INET;
	xyi.ai_socktype = SOCK_STREAM;
	xyi.ai_protocol = IPPROTO_TCP;
	xyi.ai_flags = AI_PASSIVE;
#else
	xyi.sin_family = AF_INET;
	xyi.sin_addr.s_addr = INADDR_ANY;
	xyi.sin_port = htons(port);
#endif
#ifdef WINDOWS
	if (getaddrinfo(nullptr, std::to_string(port).c_str(), &xyi, &addr) != 0) {
		throw  CraftgameException((std::string("Failed to get address info. Error code: ") + std::to_string(WSAGetLastError())).c_str());
	}
	s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (s == INVALID_SOCKET) {
		throw  CraftgameException((std::string("Failed to create socket. Error code: ") + std::to_string(WSAGetLastError())).c_str());
	}
	
	if (::bind(s, addr->ai_addr, (int)addr->ai_addrlen) == SOCKET_ERROR) {
		throw  CraftgameException("Failed to bind socket.");
	}
#else
	xyi.sin_family = AF_INET;
	xyi.sin_addr.s_addr = INADDR_ANY;
	xyi.sin_port = htons(port);
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0) {
                throw  CraftgameException("Failed to create socket.");
        }
	int opt = true;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
        if (::bind(s, (sockaddr*)&xyi, sizeof(sockaddr)) < 0) {
                throw  CraftgameException(std::string("Failed to bind to port ") + std::to_string(port) + "; does this port is unused?");
        }
#endif
}
void ServerSocket::close() {
#ifdef WINDOWS
	closesocket(s);
	WSACleanup();
#else
	shutdown(s, 2);
#endif
}
ServerSocket::~ServerSocket() {
	close();
}
Socket* ServerSocket::accept() {
	if (!this)
		return nullptr;
#ifdef WINDOWS
	if (listen(s, SOMAXCONN)) {
		throw  CraftgameException((std::string("Failed to listen socket. Error code: ") + std::to_string(WSAGetLastError())).c_str());
	}
#else
	if (listen(s, SOMAXCONN)) {
		throw  CraftgameException("Failed to listen socket.");
	}
#endif
	sockaddr_in client;
	socklen_t clilen = sizeof(sockaddr_in);
	int a = ::accept(s, (sockaddr*)&client, &clilen);
	Socket* sc = new Socket(a);
	sc->server = client;
	return sc;
}
