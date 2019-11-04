#include  "ItemStackAnimatable.h"
#include "ItemAnimator.h"

void ItemStackAnimatable::animate(std::map<std::string, SkeletalAnimation>*& sk, std::map<std::string, glm::mat4>& tr)
{
	dynamic_cast<ItemAnimator*>(getItem())->animate(this);
	sk = &animatronic.mAnimations;
	tr = ItemStackAnimatable::tr;
}
