#pragma once
#include "Packet.h"
#include "ByteBuffer.h"

class PItemHeld: public Packet
{
private:
	id_type id;
	ItemStack* stack;
	ByteBuffer buf;
public:

	PItemHeld()
		: Packet(0x19)
	{
	}
	PItemHeld(EntityPlayerMP* p, ItemStack* s):
	PItemHeld()
	{
		id = p->getId();
		stack = s;
	}

	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
