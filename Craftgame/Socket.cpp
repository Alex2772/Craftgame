#include "Socket.h"
#include <string>
#include <sstream>
#include <vector>
#include "Parsing.h"
#include <curl/curl.h>
#include "global.h"
#include "GameEngine.h"

#pragma warning(disable : 4996)

#ifdef WINDOWS
bool wsastarted = false;
#endif

uint64_t Socket::Address::toLong() const
{
	uint64_t r = sock.sin_addr.s_addr;
	r <<= 16;
	r |= sock.sin_port;
	return r;
}

Socket::Address::Address()
{
}

Socket::Address::Address(const std::string& addr, uint16_t port)
{
	memset((char *)&sock, 0, sizeof(sock));
	hostent* addrinfo = gethostbyname(addr.c_str());
	if (!addrinfo)
		throw  std::runtime_error((std::string("Unresolved hostname: ") + addr).c_str());
	memcpy((char*)&sock.sin_addr.s_addr, (char*)addrinfo->h_addr, addrinfo->h_length);
	sock.sin_family = AF_INET;
	sock.sin_port = htons(port);
}

Socket::Address::Address(const sockaddr_in& addr):
sock(addr)
{

}

sockaddr_in& Socket::Address::addr()
{
	return sock;
}

bool Socket::Address::operator>(const Address& r) const
{
	return toLong() > r.toLong();
}
bool Socket::Address::operator<(const Address& r) const
{
	return toLong() < r.toLong();
}

std::string Socket::Address::toString()
{
	char buf[32];
	sprintf(buf, "%d.%d.%d.%d:%d", sock.sin_addr.S_un.S_un_b.s_b1, sock.sin_addr.S_un.S_un_b.s_b2, sock.sin_addr.S_un.S_un_b.s_b3, sock.sin_addr.S_un.S_un_b.s_b4, htons(sock.sin_port));
	return buf;
}

/**
 * \brief Запуск сокета - сервера
 * \param source_port 
 */
Socket::Socket(uint16_t source_port):
self("0.0.0.0", source_port)
{
#ifdef WINDOWS
	w = true;
#endif
	timeout = 30;
#ifdef WINDOWS
	if (!wsastarted)
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			throw std::runtime_error((std::string("Failed. Error code: ") + std::to_string(WSAGetLastError())).c_str());
		}
		else
		{
			wsastarted = true;
		}
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		throw std::runtime_error((std::string("Failed to create socket. Error code: ") + std::to_string(WSAGetLastError())).c_str());
	}
#else
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		throw CraftgameException("Failed to create socket.");
	}
#endif
	INFO(std::string("Binding socket ") + self.toString());
	if (::bind(s, (sockaddr*)&self.addr(), sizeof(sockaddr_in)) < 0)
	{
		throw std::runtime_error((std::string("Failed to bind socket. Error code: ") + std::to_string(WSAGetLastError())).c_str());
	}
	
	static const int buflen = 524288;
	if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&buflen, sizeof(buflen)) < 0 || setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&buflen, sizeof(buflen)) < 0)
		throw std::runtime_error((std::string("Failed to setsockopt. Error code: ") + std::to_string(WSAGetLastError())).c_str());
}

/**
 * \brief Запуск сокета-клиента
 * \param addr Куда подключаться
 * \param port Порт
 */
Socket::Socket(std::string addr, uint16_t port):
	Socket((rand() % 20000) + 30000)
{
	remote = Address(addr, port);
}

Socket::~Socket()
{
	if (s)
		close();
}

void Socket::close()
{
	closesocket(s);
	//shutdown(s, 2);
	s = 0;
}

/**
 * \brief Отправляет данные по адресу. Плюёт исключение, если что-то пошло не так
 * \param buf Буфер
 * \param dst Адрес доставки
 */
void Socket::write(const ByteBuffer& buf, Address& dst)
{
	assert(buf.size() < 32768);
	static std::mutex m;
	std::unique_lock<std::mutex> lock(m);
	if (sendto(s, buf.get(), buf.size(), 0, (sockaddr*)&dst.addr(), sizeof(sockaddr_in)) <= 0)
	{
		throw CraftgameException(this, std::string("sendto error ") + std::to_string(WSAGetLastError()).c_str());
	}
}

/**
 * \brief В случае, если сокет является "клиентским", можно не указывать конечный адрес доставки
 * \param buf Буфер
 */
void Socket::write(const ByteBuffer& buf)
{
	write(buf, remote);
}

/**
 * \brief 
 * \param buf 
 * \param dst 
 */
void Socket::read(ByteBuffer& buf, Address& dst)
{
	buf.reserve(32768);
	sockaddr_in from;
	memset(&from, 0, sizeof(sockaddr_in));
	int l = sizeof(from);
	
	int res = recvfrom(s, buf.get(), buf.reserved(), 0, (sockaddr*)&from, &l);
	if (res <= 0)
	{
		throw CraftgameException(this, std::string("recvfrom error ") + std::to_string(WSAGetLastError()).c_str());
	}
	buf.setSize(res);
	dst = from;
}

#include "Exceptions.h"

/*
void http_get(string address, ostream& o)
{
	boost::asio::io_service io_service;

	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(address);
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

	boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
	ctx.load_verify_file("ca.pem");

	HttpClient c(io_service, ctx, iterator, o);

	io_service.run();
}*/
struct curl_userdata
{
	std::ostream* o;
	unsigned short http_status;
};
static size_t
curl_WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	curl_userdata* stream = static_cast<curl_userdata*>(userp);
	stream->o->write((char*)(contents), realsize);
	return realsize;
}
std::string newLocation = "";
size_t curl_ProcessHeader(char* b, size_t size, size_t nitems, void *userdata) {
	size_t numbytes = size * nitems;
	std::string s = b;
	if (Parsing::startsWith(s, "HTTP"))
	{
		curl_userdata* d = (curl_userdata*)userdata;
		std::vector<std::string> splt = Parsing::splitString(s, " ");
		d->http_status = std::strtoul(splt[1].c_str(), nullptr, 0);
	} else if (Parsing::startsWith(s, "Location"))
	{
		newLocation = s.substr(10);
		if (Parsing::endsWith(newLocation, "\r\n"))
		{
			newLocation = newLocation.substr(0, newLocation.length() - 2);
		}
	}
	return numbytes;
}
void http_get(std::string address, std::ostream* o, int rc)
{
	if (rc > 5)
		throw HttpException("Too many redirects", 0);
	newLocation = "";
	/*
	bool secure = false;
	if (rc > 5)
		throw CraftgameException(nullptr, "HTTP socket 301 exceed");
	if (Parsing::startsWith(address, "http:"))
	{
		address = address.substr(7);
	}
	else if (Parsing::startsWith(address, "https:"))
	{
		address = address.substr(8);
		secure = true;
	}
	std::string spl = Parsing::splitString(address, "/")[0];
	if (secure)
	{
		return;
	}
	Socket s(80);
	s.writeString("GET ");
	s.writeString(address.substr(spl.size()));
	s.writeString(" HTTP/1.1\r\n");
	s.writeString("Host: " + spl + "\r\n");
	s.writeString("Connection: keep-alive\r\n");
	s.writeString("Accept-Charset: utf-8\r\n");
	s.writeString("Accept: *//*\r\n\r\n");
	stringstream ss;
	size_t contentLength = 0;
	while (1)
	{
		char* c = new char;
		size_t bytes_read = s.readBytes(c, 1, 0);
		ss.write(c, 1);
		delete c;
		if (!ss.str().empty() && ss.str().find("\r\n\r\n") != std::string::npos)
		{
			string line;
			while (Parsing::getLine(ss, line, "\r\n"))
			{
				if (line.find("HTTP") == 0)
				{
					vector<string> s = Parsing::splitString(line, " ");
					if (s.size() < 3)
						throw CraftgameException("Incorrect HTTP head");
					if (s[1] == "301") // 301 Moved Temporary
					{
						while (Parsing::getLine(ss, line, "\r\n"))
						{
							if (Parsing::startsWith(line, "Location:"))
							{
								http_get(line.substr(10), o, rc + 1);
							}
						}
						throw HttpException("Location not found", stoi(s[1]));
					}
					if (s[1] != "200")
						throw HttpException((string("HTTP status ") + line + " " + address).c_str(), stoi(s[1]));
				}
				else if (line.find("Content-Length:") == 0)
				{
					vector<string> s = Parsing::splitString(line, " ");
					if (s.size() != 2)
						throw CraftgameException("Incorrect HTTP Content-Length");
					stringstream ss(s[1]);
					ss >> contentLength;
				}
			}
			break;
		}
	}
	s.read(o, [&](size_t bytes)
       {
	       return bytes < contentLength;
       });
	s.close(); */
	curl_userdata data;
	data.o = o;
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, address.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)(&data));
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36");
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*)(&data));
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_ProcessHeader);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	if (!newLocation.empty())
	{
		http_get(newLocation, o, rc--);
		return;
	}
	if (res != CURLE_OK)
	{
		throw CraftgameException(curl, std::string("cURL failed: ") + curl_easy_strerror(res));
	} 
	if (data.http_status != 200 && data.http_status != 302)
	{
		throw HttpException("Status error", data.http_status);
	}

}