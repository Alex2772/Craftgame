
#ifndef SERVER
#include "StaticShader.h"
#include "GameEngine.h"
#include "Models.h"
#include "Utils.h"
StaticShader::StaticShader(std::string vertex, std::string fragment):
	Shader(vertex, fragment)
{
}
void StaticShader::uploadFromRenderer()
{
	loadVector("c", CGE::instance->renderer->color);
	loadMatrix("transform", CGE::instance->renderer->transform);
	loadInt("texturing", CGE::instance->renderer->texturing);
}
void StaticShader::start()
{
	if (CGE::instance->currentShader == this)
		return;

	if (CGE::instance->currentShader)
		CGE::instance->currentShader->stop();
	glUseProgram(Shader::program);

	CGE::instance->currentShader = this;
	CGE::instance->staticShader = this;
}

void StaticShader::stop()
{
	Shader::stop();
}
#endif
