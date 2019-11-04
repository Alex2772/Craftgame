#pragma once
#include <glm/glm.hpp>

namespace Random
{
	int nextInt();
	int nextInt(int max);
	float nextFloat();
	bool nextBool();
	double nextDouble();
	glm::vec2 nextVec2();
	glm::vec3 nextVec3();
	glm::vec4 nextVec4();
}