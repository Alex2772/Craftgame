#include "PDespawnEntity.h"
#include "GameEngine.h"

PDespawnEntity::PDespawnEntity(Entity* t):
Packet(0x0a)
{
	id = t->getId();
}

PDespawnEntity::PDespawnEntity():
Packet(0x0a)
{
}

void PDespawnEntity::onReceived()
{
#ifndef SERVER
	Entity* e = CGE::instance->thePlayer->worldObj->getEntity(id);
	e->remove();
	delete e;
#endif
}
