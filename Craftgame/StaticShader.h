#pragma once
#include "Shader.h"
#include "Material.h"

class StaticShader : public Shader {
public:
	Material* material = nullptr;
	StaticShader(std::string vertex, std::string fragment);
	void uploadFromRenderer();
	virtual void start();
	virtual void stop();
};