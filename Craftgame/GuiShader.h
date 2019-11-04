#pragma once

#include "Shader.h"

class GuiShader : public Shader {
public:
	GuiShader():
	Shader("res/glsl/guiV.glsl", "res/glsl/guiF.glsl"){
		bindAttribute("position");
		bindAttribute("texCoords");
		link();
		start();
		loadInt(getUniform("tex"), 0);
		stop();
	}

};