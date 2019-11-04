#include "Entity.h"
#include "IServer.h"
#include "GameEngine.h"
#include "PEntityMove.h"
#include "PEntityLook.h"
#include <boost/smart_ptr/make_shared.hpp>

Entity::Entity(World* w, const id_type& _id) :
	id(_id), 
#ifndef SERVER
_getMovementSmoother(glm::vec3(0.07f), glm::vec3(0.f)),
#endif
	worldObj(w),
	aabb(glm::vec3(-0.25, 0, -0.25), glm::vec3(0.25, 1.8, 0.25))
{
	adtTag = boost::make_shared<ADT::Map>();
	pos = DPos(0, 0, 0);
	motion = glm::vec3(0);

	bool b = false;
	adtTag->putNode("isFlying", boost::make_shared<ADT::Value>(true)->setValue(b));
}

void Entity::onCollision(const Pos& t)
{
}

Entity::~Entity()
{
}
const id_type& Entity::getId()
{
	return id;
}


void Entity::render()
{
#ifndef SERVER
	this->tick();
	if (CGE::instance->renderer->renderType & GameRenderer::RENDER_FIRST) {
		glm::vec3 pm = -getMovementRender() / CGE::instance->millis;
		if (glm::length(pm) > 0.001f)
		{
			if (glm::dot(pm, glm::vec3(-cos((yawHead + 180) * M_PI / 180.f), pm.y, sin((yawHead + 180) * M_PI / 180.f))) > 0.6f)
			{
				yaw = yawHead + 50;
			}
			else if (glm::dot(pm, glm::vec3(-cos((yawHead)* M_PI / 180.f), pm.y, sin((yawHead)* M_PI / 180.f))) > 0.6f)
			{
				yaw = yawHead - 50;
			}
			else if (glm::dot(pm, glm::vec3(-cos((yawHead + 90) * M_PI / 180.f), pm.y, sin((yawHead + 90) * M_PI / 180.f))) > 0.6f)
			{
				yaw = yawHead;
			}
		}
		const float f = min(CGE::instance->millis, 0.032f) / 0.064f;
		yawHeadRender += (yawHead - yawHeadRender) * f;
		yawRender += (yaw - yawRender) * f * 0.4f;
		pitchRender += (pitch - pitchRender) * f;

		posRender += (pos - posRender) * f;
	}
	if (CGE::instance->renderer->renderType & GameRenderer::RENDER_LAST)
	{
		prevPos = pos;
	}
#endif
}

void Entity::remove()
{
	if (worldObj)
		worldObj->removeEntity(this);
}

/**
 * \brief Выставляет позицию ентити, обнуляет ускорение и отправляет пакет о перемещении
 * \param p 
 */
void Entity::setPos(const DPos& p)
{
	motion = glm::vec3(0);
	pos = p;

	if (!worldObj->isRemote)
		ts<IServer>::r(CGE::instance->server)->sendPacket(new PEntityMove(this));
}

const id_type Entity::getNetId()
{
	return netIdType;
}

void Entity::setLook(float y, float p)
{
	setLookNonServer(y, p);
	if (worldObj && !worldObj->isRemote)
		ts<IServer>::r(CGE::instance->server)->sendPacket(new PEntityLook(this));
}

void Entity::setLookNonServer(float y, float p)
{
	yawHead = y;
	pitch = p;
	if (abs(yaw - yawHead) > 50)
	{
		if (yaw < yawHead)
		{
			yaw = yawHead - 50;
		}
		else
		{
			yaw = yawHead + 50;
		}
	}
}
void Entity::applyMotion(glm::vec3 _motion)
{
	motion += _motion;
}

glm::vec3 Entity::getMovementRender()
{
#ifndef SERVER
	return _getMovementSmoother.next(pos.toVec3() - posRender.toVec3());
#else
	return glm::vec3(0.f);
#endif
}

void Entity::applyMotionNonServer(glm::vec3 m)
{
	motion += m;
}

bool Entity::isOnGround()
{
	AABB a = createAABB();
	a.move(glm::vec3(0, -0.00001f, 0));
	Pos res;
	return worldObj->checkCollision(a, res);
}

bool Entity::isFlying()
{
	return adtTag->getNode("isFlying")->toValue()->getValue<bool>();
}

void Entity::setFlying(bool b)
{
	adtTag->getNode("isFlying")->toValue()->setValue(b);
}

AABB Entity::createAABB()
{
	AABB c(aabb);
	c.move(pos.toVec3());
	return c;
}

AABB Entity::getRenderAABB()
{
	return createAABB();
}

void Entity::onVelocityChanged(glm::vec3 m)
{
}
bool __collision(Entity* entity, AABB& bb, World* worldObj, DPos& pos, float& mX, float& mY, float& mZ);
bool __collision(Entity* entity, AABB& bb, World* worldObj, DPos& pos, float& mX, float& mY, float& mZ)
{
#ifndef _DEBUG
	double _h = bb.getHeight();
	double _wx = bb.getWidthX() / 2.0;
	double _wz = bb.getWidthZ() / 2.0;

	int _ch = ceil(bb.getHeight() / 0.05);

	for (int y = 0; y < _ch; y++)
	{
		for (double x = -_wx; x <= _wx; x += 0.05)
		{
			for (double z = -_wz; z <= _wz; z += 0.05)
			{
				DPos p(pos.x + x, pos.y + (y * 0.05), pos.z + z);
				Pos ceiled;

				ceiled.x = x > 0 ? ceil(p.x) - 1 : floor(p.x);
				ceiled.y = y > 0 ? ceil(p.y) - 1 : floor(p.y);
				ceiled.z = z > 0 ? ceil(p.z) - 1 : floor(p.z);
				
				Block* b = worldObj->getBlock(ceiled);
				if (b && !b->isAir())
				{
					if (mX > 0.f)
					{
						pos.x = ceiled.x - bb.getWidthX() / 2.f;
						mX = 0;
					}
					else if (mX < 0.f)
					{
						pos.x = ceiled.x + bb.getWidthX() / 2.f + 1;
						mX = 0;
					}
					if (mZ > 0.f)
					{
						pos.z = ceiled.z - bb.getWidthZ() / 2.f;
						mZ = 0;
					}
					else if (mZ < 0.f)
					{
						pos.z = ceiled.z + bb.getWidthZ() / 2.f + 1;
						mZ = 0;
					}
					if (mY > 0.f)
					{
						pos.y = ceiled.y - bb.getHeight();
						mY = 0;
					}
					else if (mY < 0.f)
					{
						pos.y = ceiled.y + 1;
						mY = 0;
					}

					entity->onCollision(ceiled);
					return true;
				}
			}
		}
	}
	#endif
	return false;
}

bool Entity::performCollisionChecks()
{

	applyMotionNonServer(-motion * (isOnGround() ? 0.3f : 0.01f));
	bool ret = false;
	DPos p = pos;
	glm::vec3 prevM = motion;
	p.y += motion.y * CGE::instance->millis;
	float kek = 0;
	float kek1 = 0;
	ret |= __collision(this, aabb, worldObj, p, kek, motion.y, kek1);

	p.x += motion.x * CGE::instance->millis;
	kek = 0;
	kek1 = 0;
	ret |= __collision(this, aabb, worldObj, p, motion.x, kek, kek1);

	p.z += motion.z * CGE::instance->millis;
	kek = 0;
	kek1 = 0;
	ret |= __collision(this, aabb, worldObj, p, kek, kek1, motion.z);

	if (p != pos) {
		pos = p;
		this->onVelocityChanged(motion - prevM);
		if (!worldObj->isRemote)
			ts<IServer>::r(CGE::instance->server)->sendPacket(new PEntityMove(this, false));
	}
	return ret;
}

/**
 * \brief Распаковывает полученные с ADT данные о энтити.
 */
void Entity::unpack()
{
}

/**
* \brief Пакует ADT данные о энтити.
*/
void Entity::pack()
{
}

/**
 * \brief Вызывается при обнаружении столкновения с другой сущностью
 * \return Обрабатывать ли физику столкновений
 */
bool Entity::onCollideWith(Entity* other)
{
	return true;
}

glm::vec3 Entity::getEyePos()
{
	DPos p = getPos();
	p.y += eyeHeight;
	return p.toVec3();
}

glm::vec3 Entity::getLookVec()
{
	float ff = cos((pitch) / 180.f * glm::pi<float>());
	return glm::normalize(glm::vec3(sin((yawHead + 180) / 180.f * glm::pi<float>()) * ff, sin((pitch) / 180.f * glm::pi<float>()), cos((yawHead + 180) / 180.f * glm::pi<float>()) * ff));
}

void Entity::setDead()
{
	worldObj->removeEntity(this);
	delete this;
}

void Entity::tick()
{
	AABB a = createAABB();
	a.move(glm::vec3(0, -0.00001f, 0));
	prevPos = pos;
	if (!worldObj->isRemote)
		if (performCollisionChecks())
		{
			inAirTicks = 0;
		} else
		{
			inAirTicks++;
		}
}
Block* Entity::getBlockLookingAt(Pos& res, Block::BlockSide& s) {
	float x = pos.x;
	float y = pos.y + eyeHeight;
	float z = pos.z;
	float oldX, oldY, oldZ;
	float ff = cosf((pitch) / 180.f * glm::pi<float>());
	for (int dist = 0; dist < 270; dist++) {
		oldX = x;
		oldY = y;
		oldZ = z;
		y += sinf((pitch) / 180.f * glm::pi<float>()) * 0.1f;
		x += sinf((yawHead + 180) / 180.f * glm::pi<float>()) * 0.1f * ff;
		z += cosf((yawHead + 180) / 180.f * glm::pi<float>()) * 0.1f * ff;
		int ffx = floorf(x);
		int ffy = floorf(y);
		int ffz = floorf(z);
		Block* b = worldObj->getBlock({ ffx, ffy, ffz });
		if (b && !b->isAir()) {
			int kx = floorf(oldX);
			int ky = floorf(oldY);
			int kz = floorf(oldZ);

			uint8_t mode = 0;
			float j = 0.f;
			float t = glm::abs(oldX - (ffx + 0.5f));
			if (t > j)
			{
				j = t;
				mode = 0;
			}
			t = glm::abs(oldY - (ffy + 0.5f));
			if (t > j)
			{
				j = t;
				mode = 1;
			}
			t = glm::abs(oldZ - (ffz + 0.5f));
			if (t > j)
			{
				j = t;
				mode = 2;
			}
			switch (mode)
			{
			case 0:
				if (kx > ffx)
				{
					s = Block::RIGHT;
				}
				else if (kx < ffx)
				{
					s = Block::LEFT;
				}
				break;
			case 1:
				if (ky > ffy)
				{
					s = Block::TOP;
				}
				else if (ky < ffy)
				{
					s = Block::BOTTOM;
				}
				break;
			case 2:
				if (kz > ffz)
				{
					s = Block::FRONT;
				}
				else
				{
					s = Block::BACK;
				}
				break;
			}
			res = { ffx, ffy, ffz };
			return b;
		}
	}
	return nullptr;
}