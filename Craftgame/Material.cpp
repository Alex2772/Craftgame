#ifndef SERVER
#include "Material.h"
#include "GameEngine.h"
#include <Windows.h>

Material::Material(_R _r, size_t _texture, size_t _normal, glm::vec4 _color, float _reflectivity, float _shineDump) :
	r(_r),
	texture(_texture),
	reflectivity(_reflectivity),
	shineDump(_shineDump),
	color(_color),
	normal(_normal)
{
}
void Material::bind() {
	static size_t r = CGE::instance->staticShader->getUniform("material.reflectivity");
	static size_t sd = CGE::instance->staticShader->getUniform("material.shineDump");
	static size_t c = CGE::instance->staticShader->getUniform("material.color");
	static size_t useNormal = CGE::instance->staticShader->getUniform("useNormal");
	static bool* sn = CGE::instance->settings->getPropertyPointer<bool>(_R("craftgame:graphics/normal"));


	if (normal && *sn) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal);
		CGE::instance->staticShader->loadInt(useNormal, 1);
	}
	else {
		CGE::instance->staticShader->loadInt(useNormal, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	CGE::instance->staticShader->loadVector(c, color);
	CGE::instance->staticShader->loadFloat(r, reflectivity);
	CGE::instance->staticShader->loadFloat(sd, shineDump);
}
#endif