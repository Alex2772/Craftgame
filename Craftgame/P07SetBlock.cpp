#include "P07SetBlock.h"
#include "ByteBuffer.h"
#include "GameEngine.h"
#include "PacketStartLogin.h"
#include "PEntityAnimation.h"
#include "Utils.h"

P07SetBlock::P07SetBlock(Block* t, const Pos& p):
	Packet(0x07),
	pos(p)
{
	if (t) {
		i = t->getId();
		d = t->getData();
	} else
	{
		i = d = 0;
	}
}

P07SetBlock::P07SetBlock(const Pos& p) :
	Packet(0x07),
	pos(p)
{
	i = d = 1; // Break
}

P07SetBlock::P07SetBlock() :
	Packet(0x07)
{
}

void P07SetBlock::write(ByteBuffer& buffer)
{
	buffer << pos;
	buffer << i << d;
}

void P07SetBlock::read(ByteBuffer& buffer)
{
	buffer >> pos >> i >> d;
}

void P07SetBlock::onReceived()
{
#ifndef SERVER
	ts<IServer>::rw s(CGE::instance->server);
	CGE::instance->thePlayer->worldObj->setBlock(CGE::instance->goRegistry->getBlock(i, d), pos);
#endif
}

void P07SetBlock::onReceived(EntityPlayerMP* p)
{
}
