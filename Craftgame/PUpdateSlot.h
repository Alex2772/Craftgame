#pragma once
#include "Packet.h"

class PUpdateSlot: public Packet
{
public:
	ItemStack* stack;
	size_t slot;
	PUpdateSlot()
		: Packet(0x15)
	{
	}

	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
