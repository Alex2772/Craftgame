#include "Material.h"
#include "GameEngine.h"

Material::Material(_R _r) :
	r(_r)
{
}

Material::~Material()
{
	if (second)
		delete second;
}

void Material::bind() {
#ifndef SERVER
	size_t sd = CGE::instance->staticShader->getUniform("material.roughness");
	size_t ref = CGE::instance->staticShader->getUniform("material.refract");
	size_t c = CGE::instance->staticShader->getUniform("material.color");
	size_t d = CGE::instance->staticShader->getUniform("material.data");
	size_t _f0 = CGE::instance->staticShader->getUniform("material.f0");
	size_t t = CGE::instance->staticShader->getUniform("layer");
	static bool* sn = CGE::instance->settings->getPropertyPointer<bool>(_R("craftgame:graphics/normal"));

	CGE::instance->renderer->material = CGE::instance->staticShader->material = this;

	size_t data = 0;
	/*
		1 - texturing
		2 - metal
		4 - nm
	*/
	if (texture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		data |= 0x01;
	}
	if (isMetal) {
		data |= 0x02;
	}
	if (normalmap && *sn) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalmap);
		data |= 0x04;
	}
	CGE::instance->staticShader->loadInt(d, data);
	CGE::instance->staticShader->loadVector(c, color);
	CGE::instance->staticShader->loadFloat(sd, roughness);
	CGE::instance->staticShader->loadFloat(ref, refract);
	CGE::instance->staticShader->loadVector(_f0, f0);
	if (second)
		second->bind();
	else
		CGE::instance->staticShader->loadInt(t, 0);
#endif
}
Material * Material::setRoughness(float s)
{
	roughness = s;
	return this;
}

Material * Material::setTexture(size_t s)
{
	texture = s;
	return this;
}


Material * Material::setColor(glm::vec4 c)
{
	color = c;
	return this;
}

Material* Material::setF0(glm::vec3 _f0)
{
	f0 = _f0;
	return this;
}

Material * Material::set2Layer(Material * s)
{
	second = s;
	return this;
}

Material * Material::setMetal(bool m)
{
	isMetal = m;
	return this;
}

Material * Material::setNormalMap(size_t n)
{
	normalmap = n;
	return this;
}

Material* Material::setRefract(float r)
{
	refract = r;
	return this;
}

SecondMaterial::SecondMaterial(_R _r):
	Material(_r)
{

}

void SecondMaterial::bind()
{
#ifndef SERVER
	size_t sd = CGE::instance->staticShader->getUniform("material2.roughness");
	size_t ref = CGE::instance->staticShader->getUniform("material2.refract");
	size_t c = CGE::instance->staticShader->getUniform("material2.color");
	size_t d = CGE::instance->staticShader->getUniform("material2.data");
	size_t t = CGE::instance->staticShader->getUniform("layer");
	size_t _f0 = CGE::instance->staticShader->getUniform("material.f0");
	static bool* sn = CGE::instance->settings->getPropertyPointer<bool>(_R("craftgame:graphics/normal"));

	CGE::instance->renderer->material = CGE::instance->staticShader->material = this;

	size_t data = 0;
	if (texture) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture);
		data |= 0x01;
	}
	if (isMetal) {
		data |= 0x02;
	}
	if (normalmap && *sn) {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, normalmap);
		data |= 0x04;
	}
	CGE::instance->staticShader->loadInt(d, data);
	CGE::instance->staticShader->loadVector(c, color);
	CGE::instance->staticShader->loadFloat(sd, roughness);
	CGE::instance->staticShader->loadFloat(ref, refract);
	CGE::instance->staticShader->loadVector(_f0, f0);
	CGE::instance->staticShader->loadInt(t, 1);
#endif
}
