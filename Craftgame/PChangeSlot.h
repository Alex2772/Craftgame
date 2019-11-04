#pragma once
#include "Packet.h"

class PChangeSlot: public Packet
{
public:
	PChangeSlot()
		: Packet(0x17)
	{
	}
	uint8_t index = 0;

	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
