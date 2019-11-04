#pragma once
#include "Packet.h"


class P02JoinWorld: public Packet
{
public:
	P02JoinWorld();
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
