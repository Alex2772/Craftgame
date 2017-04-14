#pragma once
#ifdef WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#endif
#include <string>
#include <vector>
#include "Parsing.h"
#include <functional>
#include "CraftgameException.h"

#define BUFFER_SIZE 4096


class Socket {
private:
#ifdef WINDOWS
	WSADATA wsa;
	SOCKET s;
	bool w = true;
#else
	int s;
#endif
	long timeout = 10;
public:
	Socket(std::string addr, unsigned short port = (unsigned short)80);
#ifdef WINDOWS
	Socket(SOCKET s);
#else
	Socket(int s);
#endif
	void writeString(std::string message);
	size_t readBytes(char* buf, size_t len, int flags);
	void close();
	template<typename T>
	void write(T data) {
		if (send(s, (char*)&data, sizeof(data), 0) < 0)
		{
			throw CraftgameException("Send error");
		}
	}
	void writeBytes(const char* data, size_t size);
	template<typename T>
	T read() {
		T t;
		readBytes((char*)&t, sizeof(T), 0);
		return t;
	}
	void setTimeout(long timeout);
	void read(std::ostream& o, std::function<bool(size_t)> progress) {
		size_t bytes = 0;
		while (1) {
			char* reply = new char[BUFFER_SIZE];
			size_t bytes_read = 0;
			if ((bytes_read = recv(s, reply, BUFFER_SIZE, 0)) == 0) {
				delete[] reply;
				break;
			}
			bytes += bytes_read;
			o.write(reply, bytes_read);
			delete[] reply;
			if (!progress(bytes))
				break;
		}
	}

	sockaddr_in server;
};



class ServerSocket {
private:
#ifdef WINDOWS
	WSADATA wsa;
	SOCKET s;
#else
	int s;
#endif
public:
	ServerSocket(unsigned short port = 80);
	~ServerSocket();
#ifdef WINDOWS
	addrinfo* addr;
#else
	sockaddr* addr;
#endif
	Socket* accept();
	void close();
};

void http_get(string address, ostream& o);
