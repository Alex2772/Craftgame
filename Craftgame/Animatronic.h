#pragma once
#include "AdvancedModel.h"

class Animatronic
{
public:
	typedef std::map<std::string, SkeletalAnimation> mtype;
	mtype mAnimations;

	/*
	Применяют анимации.
	name - название
	initial - 0 - пауза, 1 - проигрывание
	repeating - повторение анимации

	applyAnimation, в отличие от updateAnimation, жёстко задает время анимации
	*/
	SkeletalAnimation& applyAnimation(std::string name, char initial = 1, bool repeating = true);
	SkeletalAnimation& updateAnimation(std::string name, char initial = 1, bool repeating = true);
};
