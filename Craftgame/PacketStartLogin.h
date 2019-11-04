#pragma once

#include "Packet.h"
#include "ByteBuffer.h"

class PacketStartLogin : public Packet {
private:
	std::string login;
	size_t id;
public:
	PacketStartLogin():
		Packet(0)
	{};
	PacketStartLogin(std::string _login, size_t _id) :
		login(_login),
		id(_id),
		Packet(0)
	{

	}
	virtual Packet* createInstance() {
		return (new PacketStartLogin)->setPacketID(getPacketID());
	}
	virtual void write(ByteBuffer& buffer) {
		buffer << login;
		buffer << id;
	}
	virtual void read(ByteBuffer& buffer) {
		buffer >> login;
		buffer >> id;
	}
	virtual void onReceived() {}
	virtual void onReceived(EntityPlayerMP* p);
};