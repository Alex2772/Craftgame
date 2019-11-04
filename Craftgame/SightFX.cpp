#ifndef SERVER
#include "SightFX.h"

void SightFX::render()
{
	ParticleFX::render();
	mVelocity.x *= 1.f - CGE::instance->millis * 2.f;
	mVelocity.z *= 1.f - CGE::instance->millis * 2.f;
	/*
	glm::vec2 uv1, uv2;
	SimpleModel model({
	glm::vec4(-1.f, -1.f, 0, 1),
	glm::vec4(1.f, -1.f, 0, 1),
	glm::vec4(-1.f, 1.f, 0, 1),
	glm::vec4(1.f, 1.f, 0, 1),
	},
	{
	glm::vec2(uv1.x, uv2.y),
	glm::vec2(uv2.x, uv2.y),
	glm::vec2(uv1.x, uv1.y),
	glm::vec2(uv2.x, uv1.y),
	},
	{
	0, 1, 2, 2, 1, 3
	});*/
	float d = lifetime / maxLifetime;
	float a = glm::pow((1.f - d), 2.f);
	mColor = sightColor * a;
	//CGE::instance->particleShader->loadVector("c", c);
}

size_t SightFX::getTextureIndex()
{
	return tex;
}
#endif