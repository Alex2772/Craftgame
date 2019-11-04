#pragma once


class ByteBuffer;
class EntityPlayerMP;

class Packet {
private:
	unsigned short packetId;
public:
	virtual ~Packet() = default;

	Packet(unsigned short p) :
		packetId(p)
	{
	}
	Packet* setPacketID(unsigned short p) {
		packetId = p;
		return this;
	}
	unsigned short getPacketID() {
		return packetId;
	}
	virtual void write(ByteBuffer& buffer) = 0;
	virtual void read(ByteBuffer& buffer) = 0;
	virtual void onReceived() = 0;
	virtual void onReceived(EntityPlayerMP*) = 0;
};
namespace Packets
{
	void reg();
}
#include "EntityPlayerMP.h"