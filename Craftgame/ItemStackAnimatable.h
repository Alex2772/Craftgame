#pragma once
#include "ItemStack.h"
#include "IAnimatable.h"
#include "Animatronic.h"

class ItemStackAnimatable: public ItemStack, public IAnimatable
{
public:
	ItemStackAnimatable(Item* i): ItemStack(i) {}

	Animatronic animatronic;
	std::map<std::string, glm::mat4> tr;
	virtual ~ItemStackAnimatable() = default;

	virtual void animate(std::map<std::string, SkeletalAnimation>*& sk, std::map<std::string, glm::mat4>& tr);
};
