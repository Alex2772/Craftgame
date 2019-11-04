#pragma once
#include "Packet.h"
#include "ByteBuffer.h"

class PEntityAnimation: public Packet
{
private:
	size_t id;
	std::string name;
	char i;
	bool r;
public:
	PEntityAnimation() :
		Packet(0x0c)
	{

	}
	PEntityAnimation(Entity* e, std::string animName, char initial, bool repeat) :
		Packet(0x0c),
		name(animName),
		i(initial),
		r(repeat)
	{
		id = e->getId();
	}

	virtual void write(ByteBuffer& buffer) {
		buffer << id << name << i << r;
	}
	virtual void read(ByteBuffer& buffer) {
		buffer >> id >> name >> i >> r;
	}
	virtual void onReceived(EntityPlayerMP* s) {
		//CGE::instance->getCEServer()->dropPlayer(s->getId());
	}
	virtual void onReceived();
};
