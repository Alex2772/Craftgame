#pragma once

#include "Packet.h"

class P01Ping : public Packet {
public:
	P01Ping();
	virtual void write(ByteBuffer& buffer);
	virtual void read(ByteBuffer& buffer);
	virtual void onReceived();
	virtual void onReceived(EntityPlayerMP*);
};