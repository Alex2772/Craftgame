#pragma once

#include "Packet.h"

class PacketPing : public Packet {
public:
	PacketPing() :
		Packet(2) {

	}
	virtual Packet* createInstance();
	virtual void write(ByteBuffer& buffer);
	virtual void read(ByteBuffer& buffer);
	virtual void onReceived();
	virtual void onReceived(EntityPlayerMP*);
};