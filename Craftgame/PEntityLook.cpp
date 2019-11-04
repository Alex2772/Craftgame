#include "PEntityLook.h"
#include "GameEngine.h"

void PEntityLook::onReceived(EntityPlayerMP* s)
{

	if (s->getId() == id)
	{
		s->setLookNonServer(yaw, pitch);
		s->getNetHandler()->sendPacket(new PEntityMove(s, false));
		ts<IServer>::rw(CGE::instance->server)->sendPacketAllExcept(new PEntityLook(s), s->getNetHandler());
	}
	//CGE::instance->getCEServer()->dropPlayer(s->getId());
}

void PEntityLook::onReceived()
{
#ifndef SERVER
	if (Entity* e = CGE::instance->thePlayer->worldObj->getEntity(id))
		e->setLook(yaw, pitch);
#endif
}
