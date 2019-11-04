#pragma once
#include "Packet.h"

class PCharging: public Packet
{
private:
	id_type id;
	uint8_t state;
public:
	PCharging()
		: Packet(0x18)
	{
	}
	PCharging(EntityPlayerMP* sabj, uint8_t state);
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};