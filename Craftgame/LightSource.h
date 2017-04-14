#pragma once

#include <glm/glm.hpp>

class LightSource {
public:
	glm::vec3 pos;
	glm::vec3 color;
	LightSource(glm::vec3 pos, glm::vec3 color);

};