#include "EntityPhysics.h"
#include "GameEngine.h"

void EntityPhysics::tick()
{
	Entity::tick();
	if (!worldObj->isRemote && !isOnGround())
	{
		motion.y -= CGE::instance->millis * 9.8f;
	}
}
