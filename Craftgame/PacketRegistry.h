#pragma once

#include <vector>
class Packet;

class PacketRegistry {
private:
	std::vector<Packet*> packets;
public:
	PacketRegistry();
	~PacketRegistry();
	void registerPacket(Packet* p);
	Packet* getPacket(unsigned short p);
};