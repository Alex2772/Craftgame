#ifndef SERVER
#include "GameRenderer.h"
#include "GameEngine.h"
#include "Models.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Windows.h>

using namespace glm;

GameRenderer::GameRenderer() {

}
void GameRenderer::renderModel(SimpleModel* model) {
	glBindVertexArray(model->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->i);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawElements(GL_TRIANGLES, (GLsizei)model->indices.size(), GL_UNSIGNED_INT, (void*)0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void GameRenderer::renderModel(AdvancedModel* model, bool materialBinding) {
	for (std::vector<ModelObject*>::iterator i = model->objects.begin(); i != model->objects.end(); i++) {
		glBindVertexArray((*i)->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*i)->i);
		glEnableVertexAttribArray(0);
		if ((*i)->texCoords->size())
			glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		if (materialBinding)
			(*i)->material->bind();
		glDrawElements(GetAsyncKeyState(VK_LCONTROL) && GetAsyncKeyState(VK_LSHIFT) ? GL_LINE_STRIP : GL_TRIANGLES, (GLsizei)(*i)->indices->size(), GL_UNSIGNED_INT, (void*)0);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(2);
		if ((*i)->texCoords->size())
			glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}
}

void GameRenderer::drawRect(int x, int y, int width, int height) {
	drawRect(x, y, width, height, 1, 1);
}
void GameRenderer::drawRect(int x, int y, int width, int height, float u, float v) {
	drawRect(x, y, width, height, 0, 0, u, v);
}
void GameRenderer::drawRect(int x, int y, int width, int height, float u, float v, float u2, float v2) {
	size_t transform = CGE::instance->guiShader->getUniform("transform");
	size_t uv = CGE::instance->guiShader->getUniform("uv");
	size_t uv2 = CGE::instance->guiShader->getUniform("uv2");
	glm::vec3 vv = glm::vec3((((float)x) / GameRenderer::viewportW * 2.f - 1), (((float)y) / GameRenderer::viewportH * -2.f + 1) - ((float)height) / GameRenderer::viewportH * 2.f, 0);
	glm::mat4 m = glm::translate(glm::mat4(1.f), vv);
	m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height) / GameRenderer::viewportH * 2.f, 0));
	if (GameRenderer::transform == glm::mat4(1.0f)) {
		CGE::instance->guiShader->loadMatrix(transform, m);
	}
	else {
		CGE::instance->guiShader->loadMatrix(transform, CGE::instance->projection2 * glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -1.3)) * GameRenderer::transform * m);
	}
	CGE::instance->guiShader->loadVector(uv, glm::vec2(u, v));
	CGE::instance->guiShader->loadVector(uv2, glm::vec2(u2, v2));
	renderModel(Models::gui);
}
void GameRenderer::blur(int x, int y, int width, int height, int radius) {
	size_t c = CGE::instance->guiShader->getUniform("c");
	static size_t c2 = CGE::instance->blurShader->getUniform("c");
	static size_t transform = CGE::instance->blurShader->getUniform("transform");
	static size_t transform2 = CGE::instance->blurShader->getUniform("transform2");
	static size_t orientation = CGE::instance->blurShader->getUniform("orientation");
	CGE::instance->blurShader->start();
	CGE::instance->blurShader->setBlurRadius(radius);
	CGE::instance->secFB->begin();
	glm::vec3 vv = glm::vec3((((float)x) / GameRenderer::viewportW * 2.f - 1), (((float)y) / GameRenderer::viewportH * -2.f + 1) - ((float)height + radius) / GameRenderer::viewportH * 2.f, 0);
	glm::mat4 m = glm::translate(glm::mat4(1.f), vv);
	m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height + radius) / GameRenderer::viewportH * 2.f, 0));
	glBindTexture(GL_TEXTURE_2D, CGE::instance->mainFB->texture);
	CGE::instance->blurShader->loadFloat(orientation, 0.f);
	CGE::instance->guiShader->start();
	CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 1));
	glm::mat4 trs(GameRenderer::transform);
	GameRenderer::transform = glm::mat4(1.0);
	//drawRect(0, 0, CGE::instance->width, CGE::instance->height, 0, 1, 1, 0);
	GameRenderer::transform = trs;
	CGE::instance->blurShader->start();
	if (GameRenderer::transform == glm::mat4(1.0f)) {
		CGE::instance->blurShader->loadMatrix(transform, m);
		CGE::instance->blurShader->loadMatrix(transform2, m);
	}
	else {
		CGE::instance->blurShader->loadMatrix(transform2, CGE::instance->projection2 * glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -1.3)) * GameRenderer::transform * m);
		CGE::instance->blurShader->loadMatrix(transform, m);
	}
	renderModel(Models::gui);
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	CGE::instance->blurShader->loadFloat(orientation, 1.f);
	glBindTexture(GL_TEXTURE_2D, CGE::instance->secFB->texture);
	vv = glm::vec3((((float)x) / GameRenderer::viewportW * 2.f - 1), (((float)y) / GameRenderer::viewportH * -2.f + 1) - ((float)height) / GameRenderer::viewportH * 2.f, 0);
	m = glm::translate(glm::mat4(1.f), vv);
	m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height) / GameRenderer::viewportH * 2.f, 0));
	if (GameRenderer::transform != glm::mat4(1.0f)) {
		CGE::instance->blurShader->loadMatrix(transform2, m);
		CGE::instance->blurShader->loadMatrix(transform, CGE::instance->projection2 * glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -1.3)) * GameRenderer::transform * m);
	}
	CGE::instance->blurShader->loadVector(c2, GameRenderer::color);
	renderModel(Models::gui);
	CGE::instance->blurShader->loadVector(c2, glm::vec4(1, 1, 1, 1));
	CGE::instance->guiShader->start();
	CGE::instance->secFB->begin();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
}
void GameRenderer::blur(int x, int y, int width, int height, std::function<void()> func, int radius) {
	size_t c = CGE::instance->guiShader->getUniform("c");
	static size_t c2 = CGE::instance->blurShader->getUniform("c");
	static size_t transform = CGE::instance->blurShader->getUniform("transform");
	static size_t transform2 = CGE::instance->blurShader->getUniform("transform2");
	static size_t orientation = CGE::instance->blurShader->getUniform("orientation");
	CGE::instance->blurShader->start();
	CGE::instance->blurShader->setBlurRadius(radius);
	CGE::instance->thirdFB->begin();
	CGE::instance->blurShader->loadFloat(orientation, 0.f);
	CGE::instance->guiShader->start();
	CGE::instance->guiShader->loadInt("masking", 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CGE::instance->guiShader->loadVector(c, glm::vec4(1.f, 1.f, 1.f, 1.f));
	glm::mat4 trs(GameRenderer::transform);
	GameRenderer::transform = glm::mat4(1.0);
	glm::vec3 vv = glm::vec3(-1, 1 - ((float)height) / GameRenderer::viewportH * 2.f, 0);
	glm::mat4 m = glm::translate(glm::mat4(1.f), vv);
	m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height) / GameRenderer::viewportH * 2.f, 0));
	func();
	//drawRect(0, 0, CGE::instance->width, CGE::instance->height, 0, 1, 1, 0);
	CGE::instance->secFB->begin();
	CGE::instance->blurShader->start();
	glBindTexture(GL_TEXTURE_2D, CGE::instance->thirdFB->texture);
	CGE::instance->blurShader->loadMatrix(transform, m);
	CGE::instance->blurShader->loadMatrix(transform2, m);
	renderModel(Models::gui);
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	CGE::instance->blurShader->loadFloat(orientation, 1.f);
	glBindTexture(GL_TEXTURE_2D, CGE::instance->secFB->texture);
	GameRenderer::transform = trs;
	if (GameRenderer::transform != glm::mat4(1.0f)) {
		CGE::instance->blurShader->loadMatrix(transform2, m);
		glm::vec3 vv = glm::vec3((((float)x) / GameRenderer::viewportW * 2.f - 1), (((float)y) / GameRenderer::viewportH * -2.f + 1) - ((float)height) / GameRenderer::viewportH * 2.f, 0);
		glm::mat4 m = glm::translate(glm::mat4(1.f), vv);
		m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height) / GameRenderer::viewportH * 2.f, 0));
		CGE::instance->blurShader->loadMatrix(transform, trs * m);
	}
	CGE::instance->blurShader->loadVector(c2, GameRenderer::color);
	renderModel(Models::gui);
	CGE::instance->blurShader->loadVector(c2, glm::vec4(1, 1, 1, 1));
	CGE::instance->guiShader->start();
	CGE::instance->secFB->begin();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	CGE::instance->guiShader->loadInt("masking", 1);
}

void GameRenderer::drawString(int x, int y, std::string text, Align align, TextStyle style, glm::vec4 _color, long size, Font* font) {
	size_t color = CGE::instance->guiShader->getUniform("c");
	size_t texture = CGE::instance->guiShader->getUniform("texturing");
	static Font* d = CGE::instance->fontRegistry->getFont(_R("craftgame:default"));
	if (!font)
		font = d;
	glActiveTexture(GL_TEXTURE0);
	if (align == Align::CENTER)
		x -= font->length(text) / 2;
	else if (align == Align::RIGHT)
		x -= font->length(text) / 2;
	int advance = 0;

	CGE::instance->guiShader->loadVector(color, glm::vec4(glm::vec3(_color) * 0.2f, _color.a));
	CGE::instance->guiShader->loadInt("texturing", 2);
	if (style == SHADOW) {
		for (std::string::iterator i = text.begin(); i != text.end(); i++) {
			if (*i == ' ')
				advance += size / 1.6f;
			else {
				Character* ch = font->getCharacter(*i, size);
				if (!ch) {
					advance += size / 1.6f;
					continue;
				}
				glBindTexture(GL_TEXTURE_2D, ch->texture);

				if (advance + x > 0 && advance + x < CGE::instance->width)
					drawRect(advance + x + 2, ch->advanceY + y + 2, ch->width, ch->height);

				advance += ch->advanceX;
			}
		}
		advance = 0;
	}
	CGE::instance->guiShader->loadVector(color, _color);
	for (std::string::iterator i = text.begin(); i != text.end(); i++) {
		if (*i == ' ')
			advance += size / 1.6f;
		else {
			Character* ch = font->getCharacter(*i, size);
			if (!ch) {
				advance += size / 1.6f;
				continue;
			}
			glBindTexture(GL_TEXTURE_2D, ch->texture);
			if (advance + x > 0 && advance + x < CGE::instance->width)
				drawRect(advance + x, ch->advanceY + y, ch->width, ch->height);
			advance += ch->advanceX;
		}
	}
	CGE::instance->guiShader->loadInt("texturing", 1);
}

void GameRenderer::changeViewport(int x, int y, int width, int height) {
	if (x != viewportX || y != viewportY || width != viewportW || height != viewportH) {
		glViewport(x, y, width, height);
		//cout << x << " " << y << " " << width << " " << height << endl;
		viewportX = x;
		viewportY = y;
		viewportW = width;
		viewportH = height;
	}
}
#endif