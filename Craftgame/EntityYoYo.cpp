#include "EntityYoYo.h"
#include "GameEngine.h"
#include "EntityItem.h"
#include "TrailFX.h"
#ifndef SERVER
#include "TestFX.h"
#include "Random.h"
#endif

void EntityYoYo::returnToPlayer()
{
	rtp = 6;
	hookedInto = nullptr;
	physics = true;
}

AABB EntityYoYo::getRenderAABB()
{
#ifndef SERVER
	AABB pos1 = createAABB();
	AABB pos2;
	if (boost::shared_ptr<ADT::Node> o = adtTag->getNode("o"))
	{
		if (boost::shared_ptr<ADT::Value> v = o->toValue())
		{
			id_type id = v->getValue<id_type>();
			if (Entity* e = CGE::instance->thePlayer->worldObj->getEntity(id))
			{
				pos2 = e->createAABB();
			}
		}
	}
	return AABB((glm::min)(pos1.getPos() - pos1.getDimensions(), pos2.getPos() - pos2.getDimensions()), (glm::max)(pos1.getPos() + pos1.getDimensions(), pos2.getPos() + pos2.getDimensions()));
#else
	return Entity::getRenderAABB();
#endif
}

bool EntityYoYo::onCollideWith(Entity* other)
{
	if (hookedInto == nullptr && other != player) {
		hookedInto = other;
		physics = false;
	}
	return other != player && other != hookedInto;
}

void EntityYoYo::tick()
{
	if (rtp > 0)
	{
		if (--rtp == 1)
		{
			setPos(player->getPos());
		}
	}
	if (hookedInto)
	{
		setPos(hookedInto->createAABB().getPos());
	}
	if (physics)
		EntityPhysics::tick();
}
EntityYoYo::EntityYoYo(EntityPlayerMP* p):
	EntityYoYo()
{
	player = p;
}

void EntityYoYo::render()
{
#ifndef SERVER
	DPos prev = posRender + glm::vec3(0, aabb.getHeight() / 2, 0);
	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->setColor(glm::vec4(1.f));
	static Material* mMaterial = CGE::instance->materialRegistry->getMaterial("yoyo");
	static AdvancedAssimpModel* model = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel(_R("yoyo.fbx")));
	mMaterial->bind();
	std::map<std::string, glm::mat4> tr;
	glm::vec3 mot = posRender.toVec3() - prevPos.toVec3();
	if (boost::shared_ptr<ADT::Node> o = adtTag->getNode("o"))
	{
		if (boost::shared_ptr<ADT::Value> v = o->toValue())
		{
			id_type id = v->getValue<id_type>();
			if (Entity* e = CGE::instance->thePlayer->worldObj->getEntity(id))
			{
				glm::vec2 n = (glm::vec2(mot.x, mot.z));
				float a = -float(std::atan2(n.y, n.x));
				if (a != a)
				{
					a = prevAngle;
				} else
				{
					prevAngle = a;
				}
				tr["base"] = glm::rotate(tr["base"], a, glm::vec3(0, 1, 0));
				if (EntitySkeleton* sk = dynamic_cast<EntitySkeleton*>(e))
					tr["ring"] = glm::translate(glm::translate(glm::mat4(), -getPos().toVec3()) * sk->getArmTransform(), glm::vec3(0, -.5f, 0));
			}
		}
	}
	tr["base"] = glm::rotate(tr["base"], glm::radians(-rotation), glm::vec3(1, 0, 0));
	rotation += glm::length(mot) * CGE::instance->millis * 360.f;
	rotation = fmodf(rotation, 360.f);
	
	CGE::instance->renderer->setTransform(getTransform());
	CGE::instance->renderer->renderModelAssimp(model, &a.mAnimations, tr, false);
	model->nextFrame();
	EntityPhysics::render();
	glm::vec3 r = posRender.toVec3() + glm::vec3(0, aabb.getHeight() / 2, 0);
	glm::vec3 p = prev.toVec3();
	if (glm::length(r - p) > 0.f)
		CGE::instance->thePlayer->worldObj->customParticles.push_back(new TrailFX(r, p, { 1, 1, 1, 1 }));
#endif
}

void EntityYoYo::onCollision(const Pos& p)
{
	if (!worldObj->isRemote && !everBeenInBlock) {
		std::vector<std::vector<Pos>> posz = {
			{
				{1, 0, 0},
				{-1, 0, 0},
				{0, 0, 1},
				{0, 0, -1},
				{1, 0, 1},
				{1, 0, -1},
				{-1, 0, 1},
				{-1, 0, -1}
			},
			{
				{ 1, 1, 0 },
				{ 0, 1, 0 },
				{ -1, 1, 0 },
				{ -1, 0, 0 },
				{ -1, -1, 0 },
				{ 0, -1, 0 },
				{ 1, -1, 0 },
				{ 1, 0, 0 },
			},
			{
				{ 0, 1, 1 },
				{ 0, 0, 1 },
				{ 0, -1, 1 },
				{ 0, -1, 0 },
				{ 0, -1, -1 },
				{ 0, 0, -1 },
				{ 0, 1, -1 },
				{ 0, 1, 0 },
			}
		};
		for (auto x : posz)
		{
			bool blya = false;
			for (auto e : x)
			{
				if (worldObj->getBlock(p + e)->isSolid())
				{
					blya = true;
					break;
				}
			}
			if (!blya)
			{
				everBeenInBlock = true;
				physics = false;
				setPos(DPos(p) + DPos(0.5, 0.5, 0.5));
				return;
			}
		}
	}
}
