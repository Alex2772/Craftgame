#pragma once
#include "Packet.h"

class PContainerAction: public Packet
{
private:
	bool process(EntityPlayerMP*& p);
public:

	PContainerAction()
		: Packet(0x13)
	{
	}
	uint16_t count;
	size_t from;
	size_t to;
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
