#pragma once

#include <glm/glm.hpp>

class ParticleFX
{
public:
	glm::vec3 mPos;
	glm::vec3 mVelocity;
	glm::vec4 mColor = glm::vec4(1.f);
	float lifetime = 0.f;
	float maxLifetime = 2.f;

	ParticleFX(const glm::vec3& pos, const glm::vec3& velocity);
	virtual ~ParticleFX();

	virtual void render();
	virtual void physics();
};
