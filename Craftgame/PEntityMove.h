#pragma once

#include "Packet.h"
#include "ByteBuffer.h"

class PEntityMove : public Packet {
private:
	size_t id;
	DPos d;
	bool df;
public:
	PEntityMove() :
		Packet(0x09)
	{

	}
	PEntityMove(Entity* e, bool denyForce = true) :
		Packet(0x09),
		df(denyForce)
	{
		id = e->getId();
		d = e->getPos();
	}

	virtual void write(ByteBuffer& buffer) {
		buffer << id << d << df;
	}
	virtual void read(ByteBuffer& buffer) {
		buffer >> id >> d >> df;
	}
	virtual void onReceived(EntityPlayerMP* s);
	virtual void onReceived();
};
