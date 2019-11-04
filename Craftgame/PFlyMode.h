#pragma once
#include "Packet.h"


class PFlyMode : public Packet {
public:
	bool flyMode;
	PFlyMode() :
		Packet(0xf)
	{}
	PFlyMode(bool b);
	virtual void write(ByteBuffer& buffer);
	virtual void read(ByteBuffer& buffer);
	virtual void onReceived();
	virtual void onReceived(EntityPlayerMP*);
};
