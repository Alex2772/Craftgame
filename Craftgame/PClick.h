#pragma once
#include "Packet.h"
#include "def.h"

class TileEntity;

class PClick: public Packet
{
public:
	PClick()
		: Packet(0x14)
	{
	}
	PClick(uint8_t b, Pos* p, Block::BlockSide s);
	PClick(uint8_t b);
	uint8_t button;
	Pos pos;
	Block::BlockSide side;
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
