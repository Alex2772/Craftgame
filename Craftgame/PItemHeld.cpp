#include "PItemHeld.h"
#ifndef SERVER
#include "GameEngine.h"
#include "EntityOtherPlayerMP.h"
#endif

void PItemHeld::write(ByteBuffer& buffer)
{
	buffer << id << stack;
}
void PItemHeld::read(ByteBuffer& buffer)
{
	buffer >> id;
	buf = buffer;
}
void PItemHeld::onReceived(EntityPlayerMP*)
{
	
}

void PItemHeld::onReceived()
{
#ifndef SERVER
	Entity* e = CGE::instance->thePlayer->worldObj->getEntity(id);
	if (e) {
		if (EntityOtherPlayerMP* p = dynamic_cast<EntityOtherPlayerMP*>(e))
		{
			buf >> p->heldItem;
			return;
		}
	}
	delete stack;
#endif
}