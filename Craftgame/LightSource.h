#pragma once

#include <glm/glm.hpp>

class LightSource {
public:
	glm::vec3 pos;
	glm::vec3 color;
	float distance;
	LightSource(glm::vec3 pos, glm::vec3 color, float distance = 15.f);
	~LightSource();

	virtual void tick() {}
};