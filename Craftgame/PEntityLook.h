#pragma once

#include "Packet.h"
#include "ByteBuffer.h"

class PEntityLook : public Packet {
private:
	size_t id;
	float yaw;
	float pitch;
public:
	PEntityLook() :
		Packet(0x0b)
	{

	}
	PEntityLook(Entity* e) :
		Packet(0x0b)
	{
		id = e->getId();
		yaw = e->yawHead;
		pitch = e->pitch;
	}

	virtual void write(ByteBuffer& buffer) {
		buffer << id << yaw << pitch;
	}
	virtual void read(ByteBuffer& buffer) {
		buffer >> id >> yaw >> pitch;
	}
	virtual void onReceived(EntityPlayerMP* s);
	virtual void onReceived();
};
