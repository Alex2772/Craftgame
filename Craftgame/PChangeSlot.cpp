#include "PChangeSlot.h"
#include "PItemHeld.h"
#include "GameEngine.h"

void PChangeSlot::write(ByteBuffer& buffer)
{
	buffer << index;
}

void PChangeSlot::read(ByteBuffer& buffer)
{
	buffer >> index;
}

void PChangeSlot::onReceived()
{
#ifndef SERVER
	onReceived(CGE::instance->thePlayer);
#endif
}

void PChangeSlot::onReceived(EntityPlayerMP* s)
{
	s->setItemIndex(index % 9);
	ts<IServer>::r(CGE::instance->server)->sendPacketAllExcept(new PItemHeld(s, s->getHeldItem()), s->getNetHandler());
}
