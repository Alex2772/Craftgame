#pragma once
#include "Packet.h"

class POpenInventory: public Packet
{
public:
	POpenInventory():
		Packet(0x11)
	{
		
	}

	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
