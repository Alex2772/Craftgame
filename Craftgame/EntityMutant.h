#pragma once
#include "EntitySkeleton.h"

class EntityMutant: public EntitySkeleton
{
private:
#ifndef SERVER
	float offset = 0.f;
	LightSource ls;
#endif
public:

	EntityMutant()
#ifndef SERVER
		: ls(glm::vec3(0), glm::vec3(0.51f, 0.82f, 0.9f) * 3.f)
#endif

	{
		netIdType = 0x03;
		weight = 350.f;
		aabb = AABB(glm::vec3(-1.f, 0.f, -1.f), glm::vec3(1.f, 2.f, 1.f));
	}

	virtual void render() override;
	virtual void tick() override;
};
