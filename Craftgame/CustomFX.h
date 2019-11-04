#pragma once

#include <string>
#include <map>
#include "Shader.h"
#include "ParticleFX.h"

class CustomFX: public ParticleFX
{
protected:

public:
	CustomFX(const glm::vec3& pos, const glm::vec3& velocity):
		ParticleFX(pos, velocity)
	{
		
	}
	Shader* shader = nullptr;
	virtual const char* getCustomShaderCode()
	{
		return nullptr;
	}
	virtual void getCustomShaderParams(std::map<std::string, std::string>& params) {}
};
