#pragma once

#include <cstddef>
using namespace std;

class World;

class Entity {
private:
	const size_t id;
public:
	Entity(World* world, const size_t& _id);
	World* worldObj;
	virtual const size_t& getId();
};
#include "World.h"
