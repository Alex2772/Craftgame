#pragma once

#include "GameEngine.h"
#include "Packet.h"
#include "ByteBuffer.h"

class PacketDisconnected : public Packet {
private:
	std::string reason;
public:
	PacketDisconnected():
		Packet(1)
	{
		
	}
	PacketDisconnected(std::string _reason):
		reason(_reason),
		Packet(1)
	{

	}
	virtual Packet* createInstance() {
		return (new PacketDisconnected(""))->setPacketID(getPacketID());
	}

	virtual void write(ByteBuffer& buffer) {
		buffer << reason;
	}
	virtual void read(ByteBuffer& buffer) {
		buffer >> reason;
	}
	virtual void onReceived(EntityPlayerMP*) {

	}
	virtual void onReceived() {
#ifndef SERVER
		CGE::instance->disconnect(nullptr, reason);
#endif
	}
};