#pragma once
#include "LightSource.h"
#include "Utils.h"

class TorchLightSource: public LightSource
{
private:
	float m = 0.5f;
public:
	TorchLightSource(glm::vec3 pos, float distance = 15.f):
		LightSource(pos, glm::vec3(0), distance)
	{
		
	}
	virtual void tick();
};
