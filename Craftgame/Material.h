#pragma once
#include "Res.h"
#ifndef SERVER

#include "gl.h"
#endif
#include <glm/glm.hpp>

class SecondMaterial;

class Material {
public:
	Material* second = nullptr;
	_R r;
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	Material(_R _r);
	~Material();
	float roughness = 1.f;
	float ior = 1.1f;
	glm::vec3 f0 = glm::vec3(0.02, 0.02, 0.02);
	size_t texture = 0;
	size_t normalmap = 0;
	float refract = 1.f;
	bool isMetal = false;

	Material* setRoughness(float s);
	Material* setTexture(size_t s);
	Material* setColor(glm::vec4 color);
	Material* setF0(glm::vec3 ior);
	Material* set2Layer(Material* s);
	Material* setMetal(bool m);
	Material* setNormalMap(size_t n);
	Material* setRefract(float r);
	virtual void bind();
};

class SecondMaterial : public Material {
public:
	SecondMaterial(_R _r);
	virtual void bind();
};
