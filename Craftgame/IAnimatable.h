#pragma once
#include <map>
#include <glm/glm.hpp>
#include "AdvancedModel.h"

class IAnimatable
{
public:
	virtual void animate(std::map<std::string, SkeletalAnimation>*& sk, std::map<std::string, glm::mat4>& tr) = 0;
};
