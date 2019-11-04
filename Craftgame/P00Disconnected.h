#pragma once

#include "GameEngine.h"
#include "Packet.h"
#include "ByteBuffer.h"

class P00Disconnected : public Packet {
private:
	std::string reason;
public:
	P00Disconnected():
		Packet(0)
	{
		
	}
	P00Disconnected(std::string _reason):

		Packet(0), reason(_reason)
	{

	}

	virtual void write(ByteBuffer& buffer) {
		buffer << reason;
	}
	virtual void read(ByteBuffer& buffer) {
		buffer >> reason;
	}
	virtual void onReceived(EntityPlayerMP* s) {
		ts<IServer>::r(CGE::instance->server)->dropPlayer(s->getId());
	}
	virtual void onReceived() {
#ifndef SERVER
		CGE::instance->disconnect(reason);
#endif
	}
};
