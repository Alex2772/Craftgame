#pragma once
#include "EntityPhysics.h"
#include "AdvancedModel.h"
#include "Animatronic.h"

class EntityYoYo: public EntityPhysics
{
private:
	EntityPlayerMP* player;
	Animatronic a;
	uint8_t skip = 60;
	uint8_t rtp = 0;
#ifndef SERVER
	float rotation = 0;
	float prevAngle = 0.f;
#endif
public:
	Entity* hookedInto = nullptr;
	bool physics = true;
	bool everBeenInBlock = false;
	EntityYoYo()
	{
		netIdType = 0x02;
		weight = 1.f;
		aabb = AABB(glm::vec3(-0.4f), glm::vec3(0.4f));
	}
	void returnToPlayer();
	virtual AABB getRenderAABB() override;
	virtual bool onCollideWith(Entity* other) override;
	virtual void tick() override;
	EntityYoYo(EntityPlayerMP* player);
	virtual void render() override;
	virtual void onCollision(const Pos& p) override;
};
