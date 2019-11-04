#ifndef SERVER
#include "TrailFX.h"
#include "GameEngine.h"

TrailFX::TrailFX(const glm::vec3& pos, const glm::vec3& pos2, glm::vec4 color): CustomFX(pos, glm::vec3(0, 0, 0)), pos2(pos2), color(color)
{
	maxLifetime = .4f;
}

const char* TrailFX::getCustomShaderCode()
{
	return "return texture(tex, vec2(abs((uv_pass.x - 0.5) * 2), uv_pass.y + life));";
}

void TrailFX::render()
{
	VAO vao;
	VAO::Loader l = vao;
	l.store(0, {
		mPos - 0.1f,
		mPos + 0.1f,
		pos2 - 0.1f,
		pos2 + 0.1f,

		mPos - glm::vec3(0.1f, -0.1f, 0.1f),
		mPos + glm::vec3(0.1f, -0.1f, 0.1f),
		pos2 - glm::vec3(0.1f, -0.1f, 0.1f),
		pos2 + glm::vec3(0.1f, -0.1f, 0.1f),
		//{0, 0, 0}
	});
	l.store(1, {
		{ 0, 0 },
		{ 1, 0 },
		{ 0, 0 },
		{ 1, 0 },
		{ 0, 0 },
		{ 1, 0 },
		{ 0, 0 },
		{ 1, 0 },
		//{ 0.5, 0.5 },
	});
	l.store(2, {
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
	});
	l.store({
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7
	});
	ParticleFX::render();
	glDisable(GL_CULL_FACE);
	shader->loadInt("tex", 0);
	static GLuint tex = CGE::instance->textureManager->loadTexture("res/particle/yoyo-trail.png", false);
	glBindTexture(GL_TEXTURE_2D, tex);
	shader->loadFloat("life", glm::clamp(lifetime / maxLifetime, 0.f, 1.f));
	vao.draw(GL_TRIANGLES);
	glEnable(GL_CULL_FACE);
}

void TrailFX::getCustomShaderParams(std::map<std::string, std::string>& params)
{
	params["tex"] = "sampler2D";
	params["life"] = "float";
}
#endif