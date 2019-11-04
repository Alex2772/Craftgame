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
#include "ByteBuffer.h"

#define BUFFER_SIZE 4096



class Socket {
public:
	class Address
	{
	private:
		sockaddr_in sock;
	public:
		uint64_t toLong() const;
		Address();
		Address(const std::string& addr, uint16_t port);
		Address(const sockaddr_in& addr);
		sockaddr_in& addr();
		bool operator>(const Address& r) const;
		bool operator<(const Address& r) const;
		std::string toString();
	};
	Socket(const Socket&) = delete;
	Socket(uint16_t port);
	Socket(std::string addr, uint16_t port);
	~Socket();
	void close();
	void write(const ByteBuffer& buf, Address& dst);
	void write(const ByteBuffer& buf);
	void read(ByteBuffer& buf, Address& dst);
	Address remote;
private:
	Address self;
#ifdef WINDOWS
	WSADATA wsa;
	SOCKET s;
	bool w = true;
#else
	int s;
#endif
	long timeout = 10;
};

void http_get(std::string address, std::ostream* o, int rc = 5);
