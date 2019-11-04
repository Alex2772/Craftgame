#pragma once
#include "EntityPhysics.h"

class EntityItem: public EntityPhysics
{
private:
	float _anim = 0.f;
	uint8_t t = 30;
public:
	ItemStack* item = nullptr;

	EntityItem():
	EntityItem(nullptr, 0, nullptr)
	{
	}

	EntityItem(World* world, const id_type& _id, ItemStack* s)
		: EntityPhysics(world, _id),
		item(s)
	{
		netIdType = 0x01;
		aabb = AABB(glm::vec3(-0.2f), glm::vec3(0.2f));
	}
	virtual ~EntityItem();
	virtual void tick();
	virtual void render();
	virtual void unpack();
	virtual void pack();

	virtual bool onCollideWith(Entity* other) override;
	virtual void onVelocityChanged(glm::vec3 m) override;
};
