#pragma once
#include "VAO.h"
#include "CustomFX.h"

class TrailFX: public CustomFX
{
private:
	glm::vec4 color;
	glm::vec3 pos2;
public:
	TrailFX(const glm::vec3& pos, const glm::vec3& pos2, glm::vec4 color);

	virtual const char* getCustomShaderCode() override;
	virtual void render() override;
	virtual void getCustomShaderParams(std::map<std::string, std::string>& params) override;
};
