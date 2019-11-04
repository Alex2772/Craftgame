#pragma once
#include "AdvancedModel.h"
#include <map>
#include "Entity.h"

class EntitySkeleton :
	public Entity
{
protected:
	glm::mat4 armTransform;
public:
	typedef std::map<std::string, SkeletalAnimation> mtype;
	AdvancedAssimpModel* model;
	/*
	Применяют анимации.
		name - название
		initial - 0 - пауза, 1 - проигрывание
		repeating - повторение анимации
	
	applyAnimation, в отличие от updateAnimation, жёстко задает время анимации
	*/
	SkeletalAnimation& applyAnimation(std::string name, char initial = 1, bool repeating = true);
	SkeletalAnimation& updateAnimation(std::string name, char initial = 1, bool repeating = true);
	EntitySkeleton() :
		model(nullptr) {}
	EntitySkeleton(World* world, const size_t& _id);
	virtual ~EntitySkeleton();
	virtual void render();
	virtual glm::mat4 getArmTransform();
protected:
	mtype mAnimations;
};

