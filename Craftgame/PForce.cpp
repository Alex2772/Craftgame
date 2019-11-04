#include "PForce.h"
#include "GameEngine.h"

void PForce::write(ByteBuffer& buffer)
{
	buffer << DPos(force);
}

void PForce::read(ByteBuffer& buffer)
{
	DPos p;
	buffer >> p;
	force = p.toVec3();
}

void PForce::onReceived()
{
#ifndef SERVER
	// Типа читер (fixed)
	if (CGE::instance->thePlayer)
		CGE::instance->thePlayer->motion = force;
#endif
}

void PForce::onReceived(EntityPlayerMP* pl)
{
	if (!pl->isFlying() && !pl->isOnGround())
	{
		force.y = 0.f;
	}
	float speed = glm::length(force);
	if (speed != speed || speed == 0.f)
	{
		pl->selfMotion = glm::vec3(0.f);
		return;
	}
	if (speed > 1.f)
		force /= speed;
	pl->selfMotion = force;
}
