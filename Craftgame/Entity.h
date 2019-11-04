#pragma once
#include  "def.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AABB.h"
#include "ADT.h"
#include "TileEntity.h"
#include "Smoother.h"

class World;
typedef size_t id_type;
class Entity {
private:
	id_type id;
#ifndef SERVER
	Smoother<glm::vec3> _getMovementSmoother;
#endif
protected:
	DPos prevPos;
	DPos pos;
	uint32_t inAirTicks = 0;
	id_type netIdType = 0;

	AABB aabb;
public:
	DPos posRender;
	boost::shared_ptr<ADT::Map> adtTag;

	float yaw = 0.f;
	float yawHead = 0.f;
	float pitch = 0.f;
	float eyeHeight = 1.625f;
	float weight = 1.f;

	glm::vec3 motion;


#ifndef SERVER
	float yawRender = 0.f;
	float yawHeadRender = 0.f;
	float pitchRender = 0.f;
#endif

	virtual ~Entity();
	Entity():
		Entity(nullptr, 0)
	{}
	Entity(World* world, const id_type& _id);
	/**
	 * \brief Коллизия с миром
	 */
	virtual void onCollision(const Pos& pos);
	virtual void tick();

	virtual void setPosNonServer(const DPos& po)
	{
		pos = po;
	}
	glm::mat4 getTransform()
	{
		return glm::translate(glm::mat4(1.0), glm::vec3(posRender.x, posRender.y, posRender.z)); 
	}
	void setId(const id_type i)
	{
		id = i;
	}

	const glm::vec3& getMotion()
	{
		return motion;
	}
	World* worldObj;
	virtual const id_type& getId();
	virtual void render();
	/**
	 * \brief Удаляет ентити из мира, но не освобождает память!
	 */
	void remove();
	virtual void setPos(const DPos& p);
	const DPos& getPos()
	{
		return pos;
	}
	const DPos& getPrevPos()
	{
		return prevPos;
	}
	const id_type getNetId();
	virtual void setLook(float yaw, float pitch);
	virtual void setLookNonServer(float yaw, float pitch);
	virtual void applyMotion(glm::vec3 _motion);
	virtual glm::vec3 getMovementRender();
	void applyMotionNonServer(glm::vec3 m);

	bool isOnGround();

	bool isFlying();

	virtual void setFlying(bool b);
	virtual void unpack();
	virtual void pack();
	virtual bool onCollideWith(Entity* other);
	glm::vec3 getEyePos();
	glm::vec3 getLookVec();
	void setDead();

	AABB createAABB();
	virtual AABB getRenderAABB();

	virtual void onVelocityChanged(glm::vec3 m);
	virtual bool performCollisionChecks();

	Block* getBlockLookingAt(Pos& res, Block::BlockSide& s);
};
#include "World.h"
