#pragma once
#include "StandartFX.h"
#include "GameEngine.h"
#include "Models.h"
#include "TextureAtlas.h"
#include "VAO.h"

class PhysicsFX : public StandartFX
{
private:
	glm::vec4 sightColor;
	size_t tex;
	float weight;
public:

	PhysicsFX(size_t texture, const glm::vec3& pos, const glm::vec3& velocity, glm::vec4 sc, float w, float lf)
		: StandartFX(pos, velocity)
	{
		maxLifetime = lf;
		sightColor = sc;
		tex = texture;
		weight = w;
	}

	virtual void render() override;

	virtual size_t getTextureIndex() override;
};
