#pragma once

#include <vector>
#include "Instancer.h"
class Packet;

class PacketRegistry {
private:
	std::vector<InstancerBase*> packets;
public:
	PacketRegistry();
	~PacketRegistry();
	void registerPacket(InstancerBase*);
	template<typename T>
	void registerPacket()
	{
		registerPacket(new Instancer<T>);
	}
	Packet* instancePacket(unsigned short p);
};