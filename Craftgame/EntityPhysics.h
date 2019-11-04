#pragma once
#include "Entity.h"

class EntityPhysics: public Entity
{
public:

	EntityPhysics()
	{
	}

	EntityPhysics(World* world, const id_type& _id)
		: Entity(world, _id)
	{
	}

	virtual void tick() override;
};