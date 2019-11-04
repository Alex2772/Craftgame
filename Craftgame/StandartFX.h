#pragma once

#include "ParticleFX.h"

class StandartFX: public ParticleFX
{
public:
	StandartFX(const glm::vec3& pos, const glm::vec3& velocity):
		ParticleFX(pos, velocity)
	{
		
	}
	virtual size_t getTextureIndex() = 0;
};