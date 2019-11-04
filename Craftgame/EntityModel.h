#pragma once

#include "EntitySkeleton.h"

class EntityModel : public EntitySkeleton {
public:
	EntityModel();
	EntityModel(World* w, const size_t& _id);
	virtual void render();
};