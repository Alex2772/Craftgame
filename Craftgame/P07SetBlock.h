#pragma once
#include "Packet.h"
#include "def.h"
#include "TileEntity.h"

class P07SetBlock : public Packet
{
private:
	bid i;
	byte d;
	Pos pos;
public:
	P07SetBlock(Block* te, const Pos& p);
	P07SetBlock(const Pos&);
	P07SetBlock();
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*);
};
