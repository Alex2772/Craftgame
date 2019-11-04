#pragma once
#include "Packet.h"

class PWorldTime: public Packet
{
private:
	world_time time;
public:
	PWorldTime()
		: Packet(0x0d)
	{
	}
	PWorldTime(world_time t)
		: Packet(0x0d),
		time(t)
	{
	}

	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
