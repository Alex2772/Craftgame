#pragma once
#include "Res.h"
#include <GL/glew.h>
#include <GL\GL.h>
#include <glm/glm.hpp>

class Material {
public:
	_R r;
	glm::vec4 color;
	Material(_R _r, size_t _texture = 0, size_t normal = 0, glm::vec4 color = glm::vec4(1, 1, 1, 1), float _reflectivity = 0.f, float _shineDump = 1.f);
	float reflectivity;
	float shineDump;
	size_t texture;
	size_t normal;

	void bind();
};