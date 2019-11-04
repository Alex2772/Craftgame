#pragma once
#include "StandartFX.h"
#include "GameEngine.h"
#include "Models.h"
#include "TextureAtlas.h"
#include "VAO.h"

class SightFX : public StandartFX
{
private:
	glm::vec4 sightColor;
	size_t tex;
public:

	SightFX(size_t texture, const glm::vec3& pos, const glm::vec3& velocity, glm::vec4 sc)
		: StandartFX(pos, velocity)
	{
		sightColor = sc;
		tex = texture;
	}

	virtual void render() override;

	virtual size_t getTextureIndex() override;
};
