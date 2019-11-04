#ifndef SERVER

#include "ParticleFX.h"
#include "GameEngine.h"

ParticleFX::ParticleFX(const glm::vec3& pos, const glm::vec3& velocity)
	: mPos(pos),
	mVelocity(velocity)
{
}
ParticleFX::~ParticleFX()
{
}
void ParticleFX::render()
{
	lifetime += CGE::instance->millis;
	this->physics();
}

void ParticleFX::physics()
{
	mPos += mVelocity * CGE::instance->millis;
}
#endif