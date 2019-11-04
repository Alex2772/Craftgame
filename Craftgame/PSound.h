#pragma once
#include "Packet.h"

class PSound: public Packet
{
private:
	std::string name;
	DPos pos;
public:
	PSound()
		: Packet(0x16)
	{
	}
	PSound(std::string n, DPos p)
		: PSound()
	{
		name = n;
		pos = p;
	}

	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
