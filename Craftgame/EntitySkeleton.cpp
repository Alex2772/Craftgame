#include "EntitySkeleton.h"
#include "GameEngine.h"
#include "PEntityAnimation.h"

EntitySkeleton::EntitySkeleton(World* world, const size_t& _id) :
	Entity(world, _id), model(nullptr)
{
}


EntitySkeleton::~EntitySkeleton()
{
}
glm::mat4 EntitySkeleton::getArmTransform()
{
	return armTransform;
}

SkeletalAnimation& EntitySkeleton::applyAnimation(std::string name, char initial, bool repeating)
{
	if (worldObj && !worldObj->isRemote)
		ts<IServer>::rw(CGE::instance->server)->sendPacket(new PEntityAnimation(this, name, initial, repeating));
	mtype::iterator fnd = mAnimations.find(name);
	if (fnd == mAnimations.end()) {
		SkeletalAnimation s;
		s.state = initial;
		if (repeating)
			s.state |= 0x80;
		mAnimations[name] = s;
		return mAnimations[name];
	}
	else {
		SkeletalAnimation& s = fnd->second;
		s.state = initial;
		if (repeating)
			s.state |= 0x80;
		s.time = 0.f;
		s.value = 0.f;
		return s;
	}
}
SkeletalAnimation& EntitySkeleton::updateAnimation(std::string name, char initial, bool repeating)
{
	mtype::iterator fnd = mAnimations.find(name);
	if (fnd == mAnimations.end()) {
		SkeletalAnimation s;
		s.state = initial;
		if (repeating)
			s.state |= 0x80;
		mAnimations[name] = s;
		return mAnimations[name];
	}
	else {
		SkeletalAnimation& s = fnd->second;
		s.state = initial;
		if (repeating)
			s.state |= 0x80;
		if (s.value == 0.f) {
			s.time = 0.f;
		}
		return s;
	}
}

void EntitySkeleton::render()
{
	Entity::render();
#ifndef SERVER
	if (CGE::instance->renderer->renderType & GameRenderer::RENDER_FIRST) {
		glm::vec3 motion = getMovementRender() * 4.f;
		float hSpeed;
		if (isOnGround() && (hSpeed = glm::length(glm::vec2(motion.x, motion.z))) > .1f)
		{
			updateAnimation("move", 1, true).speed = hSpeed * 4.f;
			updateAnimation("stay", 0, true);
		}
		else
		{
			updateAnimation("move", 0, true);
			updateAnimation("stay", 1, true);
		}
	}
#endif
}
