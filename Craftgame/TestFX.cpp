#ifndef SERVER
#include "TestFX.h"

void TestFX::render()
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
	mColor = glm::mix(glm::mix(glm::vec4(1.f, 1.f, 1.f, a), glm::vec4(1.f, 1.f, 0.f, a), glm::clamp(d * 4.f, 0.f, 1.f)), glm::vec4(1.f, 0.f, 0.f, a), d * 2);
	//CGE::instance->particleShader->loadVector("c", c);
}

size_t TestFX::getTextureIndex()
{
	return 0;
}
#endif