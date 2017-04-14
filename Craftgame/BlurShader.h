#pragma once
#ifndef SERVER

#include "Shader.h"

class BlurShader : public Shader {
private:
	int radius;
public:
	BlurShader(std::string v, std::string f);
	void setBlurRadius(int radius);
};

#endif