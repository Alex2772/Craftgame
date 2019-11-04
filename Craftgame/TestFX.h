#pragma once
#include "StandartFX.h"
#include "GameEngine.h"
#include "Models.h"
#include "TextureAtlas.h"
#include "VAO.h"

class TestFX: public StandartFX
{
public:

	TestFX(const glm::vec3& pos, const glm::vec3& velocity)
		: StandartFX(pos, velocity)
	{
	}

	virtual void render() override;

	virtual size_t getTextureIndex() override;
};
