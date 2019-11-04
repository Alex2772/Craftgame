#include "PEntityAnimation.h"
#include "GameEngine.h"

void PEntityAnimation::onReceived()
{
#ifndef SERVER
	if (Entity* e = CGE::instance->thePlayer->worldObj->getEntity(id))
	{
		if (EntitySkeleton* sk = dynamic_cast<EntitySkeleton*>(e))
		{
			sk->applyAnimation(name, i, r);
		}
	}
#endif
}
