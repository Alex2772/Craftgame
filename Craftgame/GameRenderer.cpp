#include "P07SetBlock.h"
#include "OS.h"
#include "PEntityLook.h"
#include "GuiSeekBar.h"
#include "BlockCustomRender.h"
#include "GuiInventory.h"
#include "ItemCharging.h"
#include "VAO.h"
#include <boost/smart_ptr/make_shared.hpp>
#ifndef SERVER
#include "GameRenderer.h"
#include "GameEngine.h"
#include "Models.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"
#include "Joystick.h"
#include "TextureAtlas.h"
#include "CustomShader.h"

float __swingAnim = 0;
float _3rdp_aa = 0.f;
size_t _gDrawCalls = 0;

using namespace std;
using namespace glm;

GameRenderer::GameRenderer() {

}
bool __3rd_person = false;
float* popAnim = new float[9];
void GameRenderer::drawIngameGui()
{

	setMaskingMode(0);
	setColor(glm::vec4(1));
	setTexturingMode(1);
	setTransform(glm::mat4(1.0));
	glDisable(GL_DEPTH_TEST);

	renderFlags &= ~RF_SSAO;

	CGE::instance->guiShader->start();
	if (_3rdp_aa < 1.f) {
		static GLuint t = CGE::instance->textureManager->loadTexture(_R("res/gui/crosshair.png"));
		glBindTexture(GL_TEXTURE_2D, t);
		setColor(glm::vec4(1, 1, 1, 1.f - _3rdp_aa));
		drawRect(CGE::instance->width / 2 - 16, CGE::instance->height / 2 - 16, 32, 32);
	}

	{
		static GLuint t = CGE::instance->textureManager->loadTexture(_R("res/gui/toolbar.png"), true);
		glBindTexture(GL_TEXTURE_2D, t);
		CGE::instance->maskFB->begin();
		CGE::instance->maskShader->start();
		CGE::instance->maskShader->loadInt("texturing", 1);
		setColor(glm::vec4(1));
		int startX = (CGE::instance->width - 720) / 2;
		drawRect(startX, CGE::instance->height - 80, 720, 80);
		CGE::instance->maskShader->loadInt("texturing", 0);
		CGE::instance->standartGuiShader->start();
		CGE::instance->maskFB->end();
		setTexturingMode(0);
		setMaskingMode(1);
		setColor(glm::vec4(1));
		blur(startX, CGE::instance->height - 80, 720, 80);
		setMaskingMode(0);
		setTexturingMode(1);
		glBindTexture(GL_TEXTURE_2D, t);
		setTransform(glm::mat4(1.0));
		drawRect(startX, CGE::instance->height - 80, 720, 80);
		static GLuint t2 = CGE::instance->textureManager->loadTexture(_R("res/gui/selector.png"), true);
		glBindTexture(GL_TEXTURE_2D, t2);
		drawRect(startX + CGE::instance->thePlayer->getItemIndex() * 80 - 10, CGE::instance->height - 90, 100, 100);
		GuiInventory::prepareRender();
		InventoryContainer* inv = CGE::instance->thePlayer->inventory;
		glm::mat4 pop[9];
		for (int i = 0; i < 9; i++)
		{
			float k = (1.f - (cosf(popAnim[i] * 20 / 3.14159) + 1) / 2.f) * 0.3f + 1;
			pop[i] = glm::scale(glm::mat4(1.0), glm::vec3(k, k, 1.f));
			if (popAnim[i] < 1.f)
			{
				popAnim[i] += CGE::instance->millis * 5;
			} else if (popAnim[i] > 1.f)
			{
				popAnim[i] = 1.f;
			}
		}
		for (size_t i = 0; i < 9; i++)
		{
			if (inv->slots[i]->stack)
			{
				GuiInventory::renderStack(startX + i * 80 + 14, CGE::instance->height - 80 + 14, inv->slots[i]->stack, pop[i]);
			}
		}
		GuiInventory::afterRender();
		for (size_t i = 0; i < 9; i++)
		{
			Slot*& s = inv->slots[i];
			if (s->stack && s->stack->getCount() > 1)
			{

				static CFont* f = CGE::instance->fontRegistry->getCFont("inventory");
				CGE::instance->renderer->drawString(startX + i * 80 + 14 + 47, CGE::instance->height - 80 + 14 + 49 - 16, std::to_string(s->stack->getCount()), Align::RIGHT, TextStyle::SHADOW, glm::vec4(1.f), 14, f);
			}
		}
	}
	renderFlags |= RF_SSAO;
}

void GameRenderer::renderModel(SimpleModel* model, char type) {
	glBindVertexArray(model->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->i);
	GLenum tt;
	switch (type)
	{
	case 1:
		tt = GL_LINE_LOOP;
		break;
	case 2:
		tt = GL_LINES;
		break;
	default:
		tt = GL_TRIANGLES;
	}
	glDrawElements(tt, (GLsizei)model->indices.size(), GL_UNSIGNED_INT, (void*)0);
	++_gDrawCalls;
}

void GameRenderer::renderModel(AdvancedModel* m, bool materialBinding) {
	if (AdvancedCGEMModel* model = dynamic_cast<AdvancedCGEMModel*>(m)) {
		CGE::instance->defaultStaticShader->start();
		if (CGE::instance->staticShader->material != material) {
			material->bind();
		}
		CGE::instance->staticShader->uploadFromRenderer();
		CGE::instance->camera->upload();
		for (std::vector<ModelObject*>::iterator i = model->objects.begin(); i != model->objects.end(); i++) {
			glBindVertexArray((*i)->vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*i)->i);
			glEnableVertexAttribArray(0);
			if ((*i)->texCoords->size())
				glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			if (materialBinding && (*i)->material)
				(*i)->material->bind();
			glDrawElements(GL_TRIANGLES, (GLsizei)(*i)->indices->size(), GL_UNSIGNED_INT, (void*)0);
			++_gDrawCalls;
			glDisableVertexAttribArray(3);
			glDisableVertexAttribArray(2);
			if ((*i)->texCoords->size())
				glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);
		}
	} else if (AdvancedAssimpModel* model = dynamic_cast<AdvancedAssimpModel*>(m)) {
		renderModelAssimp(model, nullptr, materialBinding);
	}
}

void GameRenderer::renderModelAssimp(AdvancedAssimpModel * model, std::map<std::string, SkeletalAnimation>* anims, bool materialBinding)
{
	std::map<std::string, glm::mat4> a;
	renderModelAssimp(model, anims, a, materialBinding);
}

std::vector<BoneTransform> transforms;
void GameRenderer::renderModelAssimp(AdvancedAssimpModel* model, std::map<std::string, SkeletalAnimation>* anims, std::map<std::string, glm::mat4>& t, bool materialBinding)
{
	if (anims) {
		CGE::instance->skeletonShader->start();
		if (!(renderType & RENDER_SHADOW) && CGE::instance->staticShader->material != material) {
			material->bind();
		}
	}
	CGE::instance->staticShader->uploadFromRenderer();
	if (renderType & RENDER_DEFAULT)
		CGE::instance->camera->upload();
	CGE::instance->staticShader->loadMatrix("transform", transform);
	if (anims) {
		transforms.clear();
		model->boneTransform(transforms, anims, t);
		for (size_t i = 0; i < transforms.size(); i++) {
			CGE::instance->staticShader->loadMatrix(string("boneMatrix[") + std::to_string(i) + "]", transforms[i].m * transforms[i].o);
		}
	}
	else if (CGE::instance->staticShader == CGE::instance->skeletonShader)
	{
		for (size_t i = 0; i < 100; i++) {
			CGE::instance->staticShader->loadMatrix(string("boneMatrix[") + std::to_string(i) + "]", glm::mat4(1.0));
		}
	}
	for (std::vector<AssimpModelObject*>::iterator i = model->mObjects.begin(); i != model->mObjects.end(); i++) {
		glBindVertexArray((*i)->vao);
		/*
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*i)->i);
		glEnableVertexAttribArray(0);
		if ((*i)->texCoords->size())
			glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		if (anims) {
			glEnableVertexAttribArray(4);
			glEnableVertexAttribArray(5);
		}*/
		if (renderType & RENDER_DEFAULT && materialBinding && (*i)->material)
			(*i)->material->bind();
		glDrawElements(GL_TRIANGLES, (GLsizei)(*i)->indices->size(), GL_UNSIGNED_INT, (void*)0);
		++_gDrawCalls;
		/*
		if (anims) {
			glDisableVertexAttribArray(5);
			glDisableVertexAttribArray(4);
		}
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(2);
		if ((*i)->texCoords->size())
			glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);*/
	}
	CGE::instance->defaultStaticShader->start();

}
void GameRenderer::renderModelAssimpOnly(AdvancedAssimpModel* model, std::string name, std::map<std::string, SkeletalAnimation>* anims, std::map<std::string, glm::mat4>& t, bool materialBinding)
{
	if (anims) {
		CGE::instance->skeletonShader->start();
		if (!(renderType & RENDER_SHADOW) && CGE::instance->staticShader->material != material) {
			material->bind();
		}
	}
	CGE::instance->staticShader->uploadFromRenderer();
	if (renderType & RENDER_DEFAULT)
		CGE::instance->camera->upload();
	CGE::instance->staticShader->loadMatrix("transform", transform);
	if (anims) {
		transforms.clear();
		model->boneTransform(transforms, anims, t);
		for (size_t i = 0; i < transforms.size(); i++) {
			CGE::instance->staticShader->loadMatrix(string("boneMatrix[") + std::to_string(i) + "]", transforms[i].m * transforms[i].o);
		}
	}
	else
	{
		for (size_t i = 0; i < 100; i++) {
			CGE::instance->staticShader->loadMatrix(string("boneMatrix[") + std::to_string(i) + "]", glm::mat4(1.0));
		}
	}
	for (std::vector<AssimpModelObject*>::iterator i = model->mObjects.begin(); i != model->mObjects.end(); i++) {
		if ((*i)->name == name) {
			glBindVertexArray((*i)->vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*i)->i);
			/*
			glEnableVertexAttribArray(0);
			if ((*i)->texCoords->size())
				glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			if (anims) {
				glEnableVertexAttribArray(4);
				glEnableVertexAttribArray(5);
			}*/
			if (renderType & RENDER_DEFAULT && materialBinding && (*i)->material)
				(*i)->material->bind();
			glDrawElements(GL_TRIANGLES, (GLsizei)(*i)->indices->size(), GL_UNSIGNED_INT, (void*)0);
			++_gDrawCalls;
			break;
		}
	}
	CGE::instance->defaultStaticShader->start();

}
/**
 * \brief Рендерит прямоугольник
 */
void GameRenderer::drawRect(int x, int y, int width, int height) {
	drawRect(x, y, width, height, 1, 1);
}

void GameRenderer::drawBorder(int x, int y, int width, int height, int w)
{
	drawRect(x, y, width, w);
	drawRect(x + width - w, y + w, w, height - w - w);
	drawRect(x, y + w, w, height - w - w);
	drawRect(x, y + height - w, width, w);

}
/**
* \brief Рендерит прямоугольник
*/
void GameRenderer::drawRect(int x, int y, int width, int height, float u, float v) {
	drawRect(x, y, width, height, 0, 0, u, v);
}


/**
* \brief Рендерит прямоугольник
*/
void GameRenderer::drawRect(float x, float y, float width, float height, float u, float v, float u2, float v2) {
	size_t transform = CGE::instance->currentShader->getUniform("transform");
	glm::mat4 m;
	if (gui3d || noAT) {
		glm::vec3 vv = glm::vec3(x, -y - height, 0);
		m = glm::translate(glm::mat4(1.f), vv);
		m = glm::scale(m, glm::vec3(width, height, 1));
	}
	else
	{
		glm::vec3 vv = glm::vec3((x / GameRenderer::viewportW * 2.f - 1), (y / GameRenderer::viewportH * -2.f + 1) - height / GameRenderer::viewportH * 2.f, 0);
		m = glm::translate(glm::mat4(1.f), vv);
		m = glm::scale(m, vec3((width) / GameRenderer::viewportW * 2.f, height / GameRenderer::viewportH * 2.f, 1));
	}
	if (GameRenderer::transform == glm::mat4(1.0f)) {
		CGE::instance->currentShader->loadMatrix(transform, m);
	}
	else if (noAT)
	{
		CGE::instance->currentShader->loadMatrix(transform, GameRenderer::transform * m);
	}
	else if (gui3d) {
		CGE::instance->currentShader->loadMatrix(transform, CGE::instance->projection * GameRenderer::transform * m);
	}
	else
	{
		CGE::instance->currentShader->loadMatrix(transform, CGE::instance->projection2 * glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -1.3)) * GameRenderer::transform * m);
	}
	size_t uv = CGE::instance->currentShader->getUniform("uv");
	size_t uv2 = CGE::instance->currentShader->getUniform("uv2");
	
	CGE::instance->currentShader->loadVector(uv, glm::vec2(u, v));
	CGE::instance->currentShader->loadVector(uv2, glm::vec2(u2, v2));
	renderModel(Models::gui);
}

/**
 * \brief Переключает зависимость UI элементов от экранных координат.
 */
void GameRenderer::setGui3D(bool f)
{
	gui3d = f;
}

void GameRenderer::drawRectNUV(int x, int y, int width, int height)
{
	drawRect(x, y, width, height);
}
void GameRenderer::drawRectUV(int x, int y, int width, int height, float u, float v)
{
	drawRect(x, y, width, height, u, v);
}
void GameRenderer::drawRectUVUV(int x, int y, int width, int height, float u, float v, float u1, float v1)
{
	drawRect(x, y, width, height, u, v, u1, v1);
}
/**
* \brief Рендерит прямоугольник. Координаты UV должны быть заданы в шейдер заранее
*/
void GameRenderer::drawTextureAtlas(int x, int y, int width, int height) {
	size_t transform = CGE::instance->guiShader->getUniform("transform");
	glm::vec3 vv = glm::vec3((((float)x) / GameRenderer::viewportW * 2.f - 1), (((float)y) / GameRenderer::viewportH * -2.f + 1) - ((float)height) / GameRenderer::viewportH * 2.f, 0);
	glm::mat4 m = glm::translate(glm::mat4(1.f), vv);
	m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height) / GameRenderer::viewportH * 2.f, 0));
	if (GameRenderer::transform == glm::mat4(1.0f)) {
		CGE::instance->guiShader->loadMatrix(transform, m);
	}
	else {
		CGE::instance->guiShader->loadMatrix(transform, CGE::instance->projection2 * glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -1.3)) * GameRenderer::transform * m);
	}
	renderModel(Models::gui);
}
void GameRenderer::blur(int x, int y, int width, int height, int radius) {
	static bool* nado = CGE::instance->settings->getPropertyPointer<bool>(_R("craftgame:gui/blur"));
	if (*nado && CGE::instance->focus) {
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
		CGE::instance->blurShader->loadInt("masking", 0);
		CGE::instance->guiShader->start();
		CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 1));
		glm::mat4 trs(GameRenderer::transform);
		GameRenderer::transform = glm::mat4(1.0);
		//drawRect(0, 0, CGE::instance->width, CGE::instance->height, 0, 1, 1, 0);
		GameRenderer::transform = trs;
		CGE::instance->blurShader->start();
		CGE::instance->blurShader->loadMatrix(transform, m);
		CGE::instance->blurShader->loadMatrix(transform2, trs * m);
		renderModel(Models::gui);
		CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
		CGE::instance->blurShader->loadFloat(orientation, 1.f);
		glBindTexture(GL_TEXTURE_2D, CGE::instance->secFB->texture);
		vv = glm::vec3((((float)x) / GameRenderer::viewportW * 2.f - 1), (((float)y) / GameRenderer::viewportH * -2.f + 1) - ((float)height) / GameRenderer::viewportH * 2.f, 0);
		m = glm::translate(glm::mat4(1.f), vv);
		m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height) / GameRenderer::viewportH * 2.f, 0));
		GameRenderer::transform = trs;
		if (GameRenderer::transform != glm::mat4(1.0f)) {
			CGE::instance->blurShader->loadMatrix(transform2, m);
			glm::vec3 vv = glm::vec3((((float)x) / GameRenderer::viewportW * 2.f - 1), (((float)y) / GameRenderer::viewportH * -2.f + 1) - ((float)height) / GameRenderer::viewportH * 2.f, 0);
			glm::mat4 m = glm::translate(glm::mat4(1.f), vv);
			m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height) / GameRenderer::viewportH * 2.f, 0));
			CGE::instance->blurShader->loadMatrix(transform, trs * m);
		}
		CGE::instance->blurShader->loadVector(c2, GameRenderer::color);
		CGE::instance->blurShader->loadInt("masking", 1);
		renderModel(Models::gui);
		CGE::instance->blurShader->loadVector(c2, glm::vec4(1, 1, 1, 1));
		CGE::instance->guiShader->start();
		CGE::instance->secFB->begin();
		CGE::instance->thirdFB->begin();
		CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
		setTransform(trs);
	}
	else {
		setColor(glm::vec4(0.1, 0.1, 0.1, 0.8) * GameRenderer::color);
		setTexturingMode(0);
		drawRect(x, y, width, height);
	}
}
/**
 * \brief Размывает то, что рендерит функция func. Требует перепросчёта маски (Gui::calculateMask) после вызова сей функции
 * \param x x конечной точки
 * \param y y конечной точки
 * \param width ширина буфера
 * \param height высота буфера
 * \param func функция
 * \param radius радиус (нечётное число до 13 включительно)
 */
void GameRenderer::blur(int x, int y, int width, int height, std::function<void()> func, int radius) {
	size_t c = CGE::instance->guiShader->getUniform("c");
	static size_t c2 = CGE::instance->blurShader->getUniform("c");
	static size_t transform = CGE::instance->blurShader->getUniform("transform");
	static size_t transform2 = CGE::instance->blurShader->getUniform("transform2");
	static size_t orientation = CGE::instance->blurShader->getUniform("orientation");
	CGE::instance->blurShader->start();
	CGE::instance->blurShader->setBlurRadius(radius);
	CGE::instance->secFB->begin();
	CGE::instance->thirdFB->begin();
	CGE::instance->blurShader->loadFloat(orientation, 0.f);
	CGE::instance->guiShader->start();
	setMaskingMode(0);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CGE::instance->guiShader->loadVector(c, glm::vec4(1.f, 1.f, 1.f, 1.f));
	glm::mat4 trs(GameRenderer::transform);
	glm::vec3 vv = glm::vec3(-1, 1 - ((float)height) / GameRenderer::viewportH * 2.f, 0);
	glm::mat4 m = glm::translate(glm::mat4(1.f), vv);
	m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height) / GameRenderer::viewportH * 2.f, 0));
	GameRenderer::transform = trs;
	func();
	//drawRect(0, 0, CGE::instance->width, CGE::instance->height, 0, 1, 1, 0);
	GameRenderer::transform = glm::mat4(1.0);
	CGE::instance->secFB->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	CGE::instance->blurShader->start();
	CGE::instance->blurShader->loadInt("masking", 0);
	glBindTexture(GL_TEXTURE_2D, CGE::instance->thirdFB->texture);
	CGE::instance->blurShader->loadMatrix(transform, m);
	CGE::instance->blurShader->loadMatrix(transform2, trs * m);
	renderModel(Models::gui);
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	CGE::instance->blurShader->loadFloat(orientation, 1.f);
	glBindTexture(GL_TEXTURE_2D, CGE::instance->secFB->texture);
	GameRenderer::transform = trs;
	//if (GameRenderer::transform != glm::mat4(1.0f))
	{
		CGE::instance->blurShader->loadMatrix(transform2, m);
		glm::vec3 vv = glm::vec3((((float)x) / GameRenderer::viewportW * 2.f - 1), (((float)y) / GameRenderer::viewportH * -2.f + 1) - ((float)height) / GameRenderer::viewportH * 2.f, 0);
		glm::mat4 m = glm::translate(glm::mat4(1.f), vv);
		m = glm::scale(m, vec3(((float)width) / GameRenderer::viewportW * 2.f, ((float)height) / GameRenderer::viewportH * 2.f, 0));
		CGE::instance->blurShader->loadMatrix(transform, trs * m);
	}
	CGE::instance->blurShader->loadVector(c2, GameRenderer::color);
	CGE::instance->blurShader->loadInt("masking", 1);
	renderModel(Models::gui);
	CGE::instance->blurShader->loadVector(c2, glm::vec4(1, 1, 1, 1));
	CGE::instance->maskShader->start();
	CGE::instance->maskFB->begin();
	setColor(glm::vec4(1.f));
	CGE::instance->guiShader->start();
	CGE::instance->secFB->begin();
	CGE::instance->thirdFB->begin();
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	setMaskingMode(1);
	setTexturingMode(1);
}

/**
 * \brief Рендерит текст
 * \param x x
 * \param y y
 * \param text текст 
 * \param align выравнивание
 * \param style стиль
 * \param _color цвет
 * \param size размер шрифта по высоте
 * \param font шрифт
 * \param formatting обрабатывать форматирование (символ параграфа)
 */
FontStyle& GameRenderer::drawString(int x, int y, std::string text, Align align, TextStyle style, glm::vec4 _color, long size, CFont* font, bool formatting) {
	FontStyle s;
	s.ts = style;
	s.color = _color;
	s.size = size;
	s.font = font;
	return drawString(x, y, text, align, s, formatting);
}
/**
* \brief Рендерит текст
* \param x x
* \param y y
* \param text текст
* \param align выравнивание
* \param fs стиль
* \param formatting обрабатывать форматирование (символ параграфа)
*/
FontStyle& GameRenderer::drawString(int x, int y, std::string text, Align align, FontStyle& fs, bool formatting)
{
	static CFont* d = CGE::instance->fontRegistry->getCFont(_R("craftgame:default"));
	if (!fs.font)
		fs.font = d;
	glActiveTexture(GL_TEXTURE0);
	if (align == Align::CENTER)
		x -= fs.font->length(text, fs.size) / 2;
	else if (align == Align::RIGHT)
		x -= fs.font->length(text, fs.size);
	int advance = 0;

	VAO vao;

	std::vector<glm::vec3> pos;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec4> colors;
	std::vector<GLuint> indices;

	glm::vec4 color(1.f);

	//if (k != 1)
	//	clr = glm::vec4(glm::vec3(fs.color) * 0.2f, fs.color.a);
	unsigned char cmd = 1;
	for (std::string::iterator i = text.begin(); i != text.end(); i++) {
		if (cmd == 2)
		{
			if (*i >= '0' && *i <= '9')
			{
				cmd = 1;
				int ctable[] = {
					0x000000FF,
					0x0000AAFF,
					0x00AA00FF,
					0x00AAAAFF,
					0xAA0000FF,
					0xAA00AAFF,
					0xFFAA00FF,
					0xAAAAAAFF,
					0x555555FF,
					0x5555FFFF
				};
				color = Utils::fromHex(ctable[*i - '0']);
			}
			else if (*i >= 'a' && *i <= 'f')
			{
				cmd = 1;
				int ctable[] = {
					0x55FF55FF,
					0x55FFFFFF,
					0xFF5555FF,
					0xFF55FFFF,
					0xFFFF55FF,
					0xFFFFFFFF
				};
				color = Utils::fromHex(ctable[*i - 'a']);
			}
		}
		else {
			if (formatting && *i == '§' && cmd == 1)
			{
				cmd = 2;
			}
			else {
				cmd = 1;
				if (*i == ' ') {
					advance += fs.size / 1.6f;
				}
				else if (*i == '\n')
				{
					advance = 0;
					y += fs.size;
					continue;
				}
				else {
					if (*i == '\\')
						cmd = 0;
					Character* ch = fs.font->getCharacter(*i & 255, fs.size);
					if (!ch) {
						advance += fs.size / 1.6f;
						continue;
					}
					if ((advance + x > 0 && advance + x < CGE::instance->width) || gui3d) {
						size_t in = pos.size();
						indices.push_back(in);
						indices.push_back(in + 1);
						indices.push_back(in + 2);
						indices.push_back(in + 2);
						indices.push_back(in + 1);
						indices.push_back(in + 3);

						pos.push_back(glm::vec3(advance, -int(ch->advanceY) - int(ch->height), 0));
						pos.push_back(glm::vec3(advance + ch->width, -int(ch->advanceY) - int(ch->height), 0));
						pos.push_back(glm::vec3(advance, -int(ch->advanceY), 0));
						pos.push_back(glm::vec3(advance + ch->width, -int(ch->advanceY), 0));

						uvs.push_back(glm::vec2(ch->uv->x, ch->uv->w));
						uvs.push_back(glm::vec2(ch->uv->z, ch->uv->w));
						uvs.push_back(glm::vec2(ch->uv->x, ch->uv->y));
						uvs.push_back(glm::vec2(ch->uv->z, ch->uv->y));

						colors.push_back(color * glm::vec4(0.7f, 0.7f, 0.7f, 1.f));
						colors.push_back(color * glm::vec4(0.7f, 0.7f, 0.7f, 1.f));
						colors.push_back(color);
						colors.push_back(color);
					}
					advance += ch->advanceX;
				}
			}
		}
	}
	{
		VAO::Loader loader = vao;
		loader.store(0, pos);
		loader.store(1, uvs);
		loader.store(2, colors);
		loader.store(indices);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}

	fs.font->textureOf(fs.size)->bind();
	std::function<void(int, int)> fun = [&](int x, int y) {
		size_t transform = CGE::instance->currentShader->getUniform("transform");
		glm::mat4 m;
		if (gui3d || noAT) {
			glm::vec3 vv = glm::vec3(x, -y - fs.size, 0);
			m = glm::translate(glm::mat4(1.f), vv);
		}
		else
		{
			glm::vec3 vv = glm::vec3((float(x) / GameRenderer::viewportW * 2.f) - 1.f, -((float(y) / GameRenderer::viewportH * 2.f) - 1.f), 0);
			m = glm::translate(glm::mat4(1.f), vv);
			m = glm::scale(m, vec3(2.f / GameRenderer::viewportW, 2.f / GameRenderer::viewportH, 1));
		}
		if (GameRenderer::transform == glm::mat4(1.0f)) {
			CGE::instance->currentShader->loadMatrix(transform, m);
		}
		else if (noAT)
		{
			CGE::instance->currentShader->loadMatrix(transform, GameRenderer::transform * m);
		}
		else if (gui3d) {
			CGE::instance->currentShader->loadMatrix(transform, CGE::instance->projection * GameRenderer::transform * m);
		}
		else
		{
			CGE::instance->currentShader->loadMatrix(transform, CGE::instance->projection2 * glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -1.3)) * GameRenderer::transform * m);
		}
	};
	setTexturingMode(2);
	if (fs.ts == TextStyle::SHADOW)
	{
		setColor(fs.color * 0.2f);
		fun(x + 2, y + 2);
		vao.draw(GL_TRIANGLES);
	}
	setColor(fs.color);
	fun(x, y);
	vao.draw(GL_TRIANGLES);

	setTexturingMode(1);
	return fs;
}

/**
 * \brief Требует перепросчёта маски (Gui::calculateMask) после каждого вызова сей функции.
 * \param x x
 * \param y y
 * \param width ширина
 * \param height высота
 * \param c цвет
 * \param spr растяжение
 * \param radius радиус размытия (влияет на качество)
 */
void GameRenderer::drawShadow(int x, int y, int width, int height, glm::vec4 c, int spr, int radius)
{
	blur(x - spr - radius, y - spr - radius, width + (radius + spr) * 2, height + (radius + spr) * 2, [&]() {
		CGE::instance->maskFB->bind(CGE::instance->width, CGE::instance->height);
		CGE::instance->guiShader = CGE::instance->maskShader;
		CGE::instance->guiShader->start();
		glm::vec4 cr = GameRenderer::color;
		GameRenderer::color = glm::vec4(1.f);
		setColor(glm::vec4(1.f));
		glm::mat4 tr = transform;
		setTransform(glm::mat4(1.0));
		drawRect(0, 0, CGE::instance->width, CGE::instance->height);
		GameRenderer::color = glm::vec4(0, 0, 0, 1.f);
		setColor(glm::vec4(1.f));
		setTransform(tr);
		drawRect(0, 0, width, height);
		CGE::instance->maskFB->end();
		CGE::instance->thirdFB->begin();
		CGE::instance->guiShader = CGE::instance->standartGuiShader;
		CGE::instance->guiShader->start();
		setTexturingMode(0);
		GameRenderer::color = cr;
		setColor(c);
		drawRect(radius, spr + radius, width + spr * 2, height);
	}, radius);
}

void GameRenderer::renderBlockIcon(Block* b, int x, int y)
{
	int p = renderType;
	renderType = 0;
	//glm::vec3 vv = glm::vec3(-1.f, ((1 - float(y) / GameRenderer::viewportH) * 2.f - 1) - 80.f / GameRenderer::viewportH * 2.f, 0);
	y += 60;
	x += 40;
	glm::vec3 vv = glm::vec3(float((x - viewportW / 2) * 2) / viewportW, float(-(y - viewportH / 2) * 2) / viewportH, 0);
	transform = glm::translate(glm::mat4(1.0), vv);
	renderBlockIcon(b);
	renderType = p;
}

void GameRenderer::renderBlock(Block* b, glm::mat4 cp) {
	if (BlockCustomRender* d = dynamic_cast<BlockCustomRender*>(b))
	{
		d->renderBlock(Pos(-1, -1, -1), cp);
		setTexturingMode(1);
		setColor(glm::vec4(1.f));
	}
	else {
		static Block::BlockSide sides[] = {
			Block::BOTTOM,
			Block::FRONT,
			Block::RIGHT,
			Block::BACK,
			Block::LEFT,
			Block::TOP,
		};
		CGE::instance->defaultStaticShader->loadInt("block", 1);
		for (byte i = 1; i < 5; i++) {
			CGE::instance->defaultStaticShader->loadMatrix("transform", glm::translate(cp = glm::rotate(cp, glm::radians(90.f), glm::vec3(0, 0.5f, 0)), glm::vec3(-0.5f, 0, 0.5f)));
			b->getMaterialBySide(sides[i])->bind();
			renderModel(Models::gui);
		}
		b->getMaterialBySide(sides[0])->bind();
		CGE::instance->defaultStaticShader->loadMatrix("transform", glm::translate(glm::rotate(cp, glm::radians(90.f), glm::vec3(0.5f, 0, 0)), glm::vec3(-0.5f, -0.5f, 0)));
		renderModel(Models::gui);
		b->getMaterialBySide(sides[5])->bind();
		CGE::instance->defaultStaticShader->loadMatrix("transform", glm::translate(glm::rotate(cp, glm::radians(-90.f), glm::vec3(0.5f, 0, 0)), glm::vec3(-0.5f, -0.5f, 1.f)));
		renderModel(Models::gui);
		CGE::instance->defaultStaticShader->loadInt("block", 0);
	}
}


/**
 * \brief Рендерит блок в изометрии. Используется в иконках
 * \param b Блок
 */
void GameRenderer::renderBlockIcon(Block * b)
{
	disablePerspective();
	glm::mat4 c = glm::scale(transform, glm::vec3(70.f / float(viewportW), 70.f / float(viewportH), 0.2f));
	c = glm::rotate(c, float(glm::radians(90.f / M_PI)), glm::vec3(0.5, 0, 0));
	c = glm::rotate(c, glm::radians(45.f), glm::vec3(0, 0.5f, 0));
	renderBlock(b, c);
	enablePerspective();
}

glm::vec4 globalGuiColor(1.f);

/**
 * \brief Выставляет текущий цвет
 * \param _c цвет
 */
void GameRenderer::setColor(glm::vec4 _c) const
{
	if (CGE::instance->currentShader == CGE::instance->guiShader || CGE::instance->currentShader == CGE::instance->blurShader)
		_c *= globalGuiColor;
	CGE::instance->currentShader->loadVector("c", color * _c);
}

/**
 * \brief Выставляет режим текстурирования
 * \param mode режим
 * 0 - без текстуры
 * 1 - с текстурой
 * 2 - с текстурой одного цвета (шрифты)
 * 3 - линейный градиент
 * 4 - судя по коду, нелинейный градиент
 */
void GameRenderer::setTexturingMode(int mode)
{
	texturing = mode;
	CGE::instance->currentShader->loadInt("texturing", mode);
}
/**
 * \brief Выставляет режим маски
 * \param mode режим
 * 0 - маска выключена
 * 1 - маска включена
 */
void GameRenderer::setMaskingMode(int mode) const
{
	CGE::instance->currentShader->loadInt("masking", mode);
}

void GameRenderer::setTransform(glm::mat4 t)
{
	transform = t;
}

void GameRenderer::enablePerspective() const
{
	CGE::instance->staticShader->loadMatrix("projection", CGE::instance->projection);
}

void GameRenderer::disablePerspective() const
{
	CGE::instance->staticShader->loadMatrix("projection", glm::ortho(-1.f, 1.f, -1.f, 1.f));
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

void GameRenderer::renderChunks(byte k)
{
	static GLuint t = CGE::instance->textureManager->loadTexture(_R("res/blocks/cobblestone.png"));
	glBindTexture(GL_TEXTURE_2D, t);
	if (k) {
		CGE::instance->renderer->setTexturingMode(1);
		CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 1));
		CGE::instance->renderer->transform = glm::mat4(1.0);
	}
	CGE::instance->staticShader->loadMatrix("transform", glm::mat4(1.0));
	size_t chunkCount = 0;
	for (Chunks::iterator i = CGE::instance->thePlayer->worldObj->renderList.begin(); i != CGE::instance->thePlayer->worldObj->renderList.end(); ++i)
	{
		AABB box = (*i)->createAABB();
		if (k || frustum.isBoxInFrustum(box) || glm::length(box.getPos() - CGE::instance->thePlayer->getPos().toVec3()) < 32.f) {
			(*i)->render(k);
			chunkCount++;
		}
	}
	CGE::instance->setDebugString("Chunks", std::to_string(chunkCount));
}
glm::mat4 lightSpaceMatrix1;
glm::mat4 lightSpaceMatrix2;
extern ItemStack* asd;
void GameRenderer::render(byte t)
{
	
	if (asd && CGE::instance->focus)
	{
		if (!(Joystick::getKeyState(Joystick::R1) || Keyboard::isKeyDown(Keyboard::RButton)) || CGE::instance->thePlayer->getHeldItem() != asd)
		{
			if (IStackCharging* kek = dynamic_cast<IStackCharging*>(asd)) {
				if (kek->isUsed)
				{
					dynamic_cast<ItemCharging*>(asd->getItem())->stopCharging(asd, CGE::instance->thePlayer);
					asd = nullptr;
				}
			}
		}
	}
	static float _3rdp_a = 0;
	glm::vec3 bobbing(0.f);
	glm::vec3 tmp;
	if (t == 0) {

		CGE::instance->skeletonShader->start();
		CGE::instance->skeletonShader->loadMatrix("projection", CGE::instance->projection);
		CGE::instance->camera->upload();
		CGE::instance->defaultStaticShader->start();
		CGE::instance->defaultStaticShader->loadMatrix("projection", CGE::instance->projection);
		CGE::instance->camera->upload();

		if (CGE::instance->thePlayer->isOnGround())
		{
			glm::vec3 motion = CGE::instance->thePlayer->getMovementRender();
			float sp = glm::sqrt(glm::length(glm::vec2(motion.x, motion.z))) * 2.f;

			static float _move_anim = 0.f;
			_move_anim += sp * CGE::instance->millis * 1.1f;
			_move_anim = fmodf(_move_anim, 1.f);

			glm::vec3 tr;
			tr.x = _move_anim * 4 - 1;
			if (tr.x > 1)
			{
				tr.x = 2.f - tr.x;
			}
			tr.y = powf(tr.x, 2);
			bobbing = tr * 0.1f * glm::min(sp, 1.f);
		}
		CGE::instance->camera->pos = CGE::instance->thePlayer->posRender.toVec3();
		CGE::instance->camera->pos.y += CGE::instance->thePlayer->eyeHeight;
		CGE::instance->thePlayer->worldObj->l.pos = CGE::instance->camera->pos;

		if (__3rd_person)
		{
			_3rdp_a += CGE::instance->millis * 1.5f;
		} else {
			_3rdp_a -= CGE::instance->millis * 1.5f;
		}
		_3rdp_a = glm::clamp(_3rdp_a, 0.f, 1.f);
		_3rdp_aa = (cos((_3rdp_a + 1) * 10 / glm::pi<float>()) + 1) / 2.f;
		//if (_3rdp_a > 0)
		{
			CGE::instance->camera->pos.z += sin((-CGE::instance->camera->yaw - 90.f) * M_PI / 180.f) * cos((CGE::instance->camera->pitch) * M_PI / 180.f) * 5 * _3rdp_aa;
			CGE::instance->camera->pos.x += cos((-CGE::instance->camera->yaw - 90.f) * M_PI / 180.f) * cos((CGE::instance->camera->pitch) * M_PI / 180.f) * 5 * _3rdp_aa;
			CGE::instance->camera->pos.y += sin((CGE::instance->camera->pitch) * M_PI / 180.f) * 5 * _3rdp_aa;
			CGE::instance->camera->yaw += 180 * _3rdp_aa;
			CGE::instance->camera->pitch -= CGE::instance->camera->pitch * 2 * _3rdp_aa;
		}

		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		CGE::instance->defaultStaticShader->start();
		CGE::instance->gBuffer->begin();
		tmp = CGE::instance->camera->pos;
		CGE::instance->camera->pos += glm::vec3(0, bobbing.y * 0.8f, 0);
		CGE::instance->camera->upload();
		//frustum.update(glm::perspective(glm::radians(110.f), float(CGE::instance->width) / float(CGE::instance->height), 0.01f, 1000.f) * CGE::instance->camera->createViewMatrix());
		frustum.update(CGE::instance->projection * CGE::instance->camera->createViewMatrix());
		Block::BlockSide sk;
		Pos p;
		if (CGE::instance->thePlayer->getBlockLookingAt(p, sk))
		{
			{
				glm::mat4 m(1.0);
				m = glm::translate(m, DPos(p).toVec3() + 0.5f);
				m = glm::scale(m, glm::vec3(.505f));
				CGE::instance->staticShader->loadMatrix("transform", m);
			}
			CGE::instance->renderer->setTexturingMode(0);
			CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 0));
			CGE::instance->renderer->renderModel(Models::selection, 2);
			CGE::instance->renderer->setTexturingMode(1);
			CGE::instance->renderer->setColor(glm::vec4(1));
		}
		std::string k = "UNK";
		switch (sk)
		{
		case Block::TOP:
			k = "TOP";
			break;
		case Block::LEFT:
			k = "LEFT";
			break;
		case Block::RIGHT:
			k = "RIGHT";
			break;
		case Block::BOTTOM:
			k = "BOTTOM";
			break;
		case Block::BACK:
			k = "BACK";
			break;
		case Block::FRONT:
			k = "FRONT";
			break;
		}
		CGE::instance->setDebugString("Block facing", k);
	}
	else {
		CGE::instance->currentShader = CGE::instance->defaultStaticShader;
		CGE::instance->staticShader = CGE::instance->defaultStaticShader;

		glm::mat4 o1 = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, 1.f, 700.f);
		glm::mat4 o2 = glm::ortho(-250.0f, 250.0f, -250.0f, 250.0f, 2.f, 700.f);

		glm::vec3 pos = CGE::instance->thePlayer->worldObj->calculateSunPos();
		pos.y = glm::abs(pos.y);
		glm::vec3 p(CGE::instance->thePlayer->getPos().toVec3() + pos * 400.f);

		glm::mat4 view = glm::lookAt(p,
			CGE::instance->thePlayer->getPos().toVec3(),
			glm::vec3(0.0f, 1.0f, 0.0f));

		lightSpaceMatrix1 = o1 * view;
		lightSpaceMatrix2 = o2 * view;

		CGE::instance->skeletonShader->start();
		CGE::instance->skeletonShader->loadMatrix("transforms[0]", lightSpaceMatrix1);
		CGE::instance->skeletonShader->loadMatrix("transforms[1]", lightSpaceMatrix2);
		CGE::instance->skeletonShader->loadMatrix("transform", glm::mat4());
		
		CGE::instance->defaultStaticShader->start();
		CGE::instance->defaultStaticShader->loadMatrix("transforms[0]", lightSpaceMatrix1);
		CGE::instance->defaultStaticShader->loadMatrix("transforms[1]", lightSpaceMatrix2);
		CGE::instance->defaultStaticShader->loadMatrix("transform", glm::mat4());
	}

	if (t == 0) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); 
	} else
	{
		glDisable(GL_CULL_FACE);
	}
	renderChunks(t);
	if (t == 0) {
		glDisable(GL_CULL_FACE);
	}

	for (Entity* e : CGE::instance->thePlayer->worldObj->entities)
	{
		if (CGE::instance->thePlayer != e && (t == 1 || frustum.isBoxInFrustum(e->getRenderAABB())))
			e->render();
	}
	CGE::instance->thePlayer->render();
	if (_3rdp_a < 1.f && t == 0) {
		CGE::instance->staticShader->use();
		glm::mat4 m(1.0);

		CGE::instance->thePlayer->itemAnim += CGE::instance->millis * 2.f;
		CGE::instance->thePlayer->itemAnim = glm::clamp(CGE::instance->thePlayer->itemAnim, 0.f, 1.f);
		static Item* prev = nullptr;
		ItemStack* cs = CGE::instance->thePlayer->getHeldItem();
		if (cs) {
			if (cs->getItem() != prev)
			{
				prev = cs->getItem();
				CGE::instance->thePlayer->itemAnimation();
			}
		} else
		{
			if (prev)
			{
				prev = nullptr;
				CGE::instance->thePlayer->itemAnimation();
			}
		}
		if (CGE::instance->thePlayer->itemAnim > 0.5f && CGE::instance->thePlayer->getPrevItemIndex() != CGE::instance->thePlayer->getItemIndex())
			CGE::instance->thePlayer->setItemIndex(CGE::instance->thePlayer->getItemIndex());
		ItemStack* c = CGE::instance->thePlayer->inventory->slots[CGE::instance->thePlayer->itemAnim < 0.5f ? CGE::instance->thePlayer->getPrevItemIndex() : CGE::instance->thePlayer->getItemIndex()]->stack;
		if (c) {
			m = glm::translate(m, CGE::instance->camera->pos);
			//m = glm::translate(m, glm::vec3(0, 60, 0));
			m = glm::rotate(m, glm::radians(CGE::instance->camera->yaw), glm::vec3(0, 1, 0));
			m = glm::rotate(m, glm::radians(CGE::instance->camera->pitch), glm::vec3(1, 0, 0));

			if (!c) {
				m = glm::translate(m, glm::vec3(-7.5f, -30.f, 0));
				//m = glm::translate(m, glm::vec3(-4.05f, (_debug_1->value - 500) / 10.f, 0));
				//m = glm::translate(m, glm::vec3((_debug_1->value - 500) / 10.f, -30.f, 0));
			}
			static float prevYaw = 0;
			static float prevPitch = 0;
			static float holdAnim = 0;
			prevYaw += (CGE::instance->thePlayer->yawHead - prevYaw) * CGE::instance->millis * 16;
			prevPitch += (CGE::instance->thePlayer->pitch - prevPitch) * CGE::instance->millis * 16;

			float swing = 1.f - __swingAnim;

			m = glm::scale(m, glm::vec3(c ? 0.1f : 14.f / 100000.f));
			if (!c)
				m = glm::translate(m, glm::vec3(5350.f, 21009.f, -105.f) * 10.f);
			m = glm::translate(m, glm::vec3(0, -_3rdp_aa * 5.f, 0));
			//m = glm::rotate(m, glm::radians((float(cos((swing - 0.5) / glm::pi<float>() * 20.f)) + 1) / 2.f * 30.f), glm::vec3(0, 1, 0));
			m = glm::rotate(m, glm::radians((float(cos((swing - 0.5) / glm::pi<float>() * 20.f)) + 1) / 2.f * 5.f), glm::vec3(0, 0, -1));
			m = glm::translate(m, glm::vec3(0.9f, -1.4f, -1.2f));
			glm::vec3 kd = CGE::instance->thePlayer->getMovementRender();
			glm::vec3 tm = glm::vec3(
				-kd.x * sin((CGE::instance->camera->yaw + 90.f) * M_PI / 180.f) -
				kd.z * cos((CGE::instance->camera->yaw + 90.f) * M_PI / 180.f),
				-kd.y,
				-kd.z * sin((-CGE::instance->camera->yaw + 90.f) * M_PI / 180.f) -
				kd.x * cos((-CGE::instance->camera->yaw + 90.f) * M_PI / 180.f)
			);
			tm = glm::sqrt(glm::abs(tm)) * glm::sign(tm) * .1f;
			tm += glm::vec3((CGE::instance->thePlayer->yawHead - prevYaw) * 0.003, (CGE::instance->thePlayer->pitch - prevPitch) * -0.003 + cos(holdAnim / 3.14 * 20) * 0.005f, 0);
			tm += bobbing;
			//tm.z += (_debug_1->value - 500) / 10.f;
			//tm.x += (_debug_1->value - 500) / 10.f;
			if (!c)
			{
				tm *= 1000.f;
			}
			m = glm::translate(m, tm);
			if (dynamic_cast<Block*>(c->getItem())) {
				m = glm::rotate(m, glm::radians(-34.f), glm::vec3(0, 1.f, 0));
				m = glm::rotate(m, glm::radians(-9.f), glm::vec3(0, 0, 1.f));
			} else
			{
				m = glm::translate(m, glm::vec3(0, -.1f, 0));
				m = glm::rotate(m, glm::radians(-74.f), glm::vec3(0, 1.f, 0));
				m = glm::rotate(m, glm::radians(-9.f), glm::vec3(0, 0, 1.f));
			}
			m = glm::scale(m, glm::vec3(0.8));
			m = glm::translate(m, glm::vec3(0, 0.5f, 0));
			if (c) {
				glm::mat4 prev = transform;

				float k = (1.f - (cosf(CGE::instance->thePlayer->itemAnim * 20 / 3.14159) + 1) / 2.f) * 0.3f;
				m = glm::translate(m, glm::vec3(0, k * -4.f, 0));
				m = glm::rotate(m, glm::radians(180.f), glm::vec3(0, 1.f, 0));
				transform = m;
				c->getItem()->getRenderer()->render(IItemRenderer::FIRST_PERSON, c->getItem(), c);
				transform = prev;
			}
			else
			{
				setTransform(m);
				static AdvancedAssimpModel* model = model = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel(_R("craftgame:player")));
				static Material* mat = CGE::instance->materialRegistry->getMaterial(_R("craftgame:skin"));
				size_t* skin = CGE::instance->skinManager.getSkin(CGE::instance->thePlayer->getGameProfile().getUsername());
				mat->bind();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, *skin);
				std::map<std::string, SkeletalAnimation> mAnimations;
				std::map<std::string, glm::mat4> transformation;
				renderModelAssimpOnly(model, "Cube.015", &mAnimations, transformation, false);
				//renderModelAssimp(model, &mAnimations, transformation, false);
			}
			if (__swingAnim > 0)
				__swingAnim -= CGE::instance->millis * 3;
			else
				__swingAnim = 0;


			holdAnim += CGE::instance->millis * 0.4f;
			holdAnim = fmod(holdAnim, 1);
		}
	}
	if (t == 0)
	{
		CGE::instance->setDebugString("x", std::to_string(CGE::instance->thePlayer->getPos().x));
		CGE::instance->setDebugString("y", std::to_string(CGE::instance->thePlayer->getPos().y));
		CGE::instance->setDebugString("z", std::to_string(CGE::instance->thePlayer->getPos().z));
		CGE::instance->setDebugString("yaw", std::to_string(CGE::instance->thePlayer->yawHead));
		CGE::instance->setDebugString("pitch", std::to_string(CGE::instance->thePlayer->pitch));
		/*
		// SKYBOX
		CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
		CGE::instance->skyboxShader->use();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
		vec3 sun = CGE::instance->thePlayer->worldObj->calculateSunPos();

		glEnable(GL_DEPTH_TEST);

		// Звёзды
		if (sun.y < 0) {
			static GLuint vao = 0;
			static size_t cnt = 0;
			if (!vao)
			{
				vao = CGE::instance->vao->createVAO();
				std::vector<glm::vec3> stars;
				stars.reserve(500);
				for (size_t i = 0; i < rand() % 100 + 300; i++)
				{
					stars.push_back(glm::vec3((rand() % 2000) / 1000.f - 1, (rand() % 2000) / 1000.f - 1, (rand() % 2000) / 1000.f - 1));
				}
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.309421, 0.609697, -0.729745));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.309421, 0.609697, -0.729745));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.309421, 0.609697, -0.729745));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.309421, 0.609697, -0.729745));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.307974, 0.583105, -0.751758));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.307974, 0.583105, -0.751758));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.307974, 0.583105, -0.751758));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.307974, 0.583105, -0.751758));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.303054, 0.550061, -0.778197));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.303054, 0.550061, -0.778197));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.303054, 0.550061, -0.778197));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.300516, 0.498098, -0.813381));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.300516, 0.498098, -0.813381));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.300516, 0.498098, -0.813381));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.300516, 0.498098, -0.813381));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.278266, 0.5631, -0.77813));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.278266, 0.5631, -0.77813));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.278266, 0.5631, -0.77813));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.24944, 0.570284, -0.78266));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.24944, 0.570284, -0.78266));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.24944, 0.570284, -0.78266));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.230249, 0.620695, -0.749482));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.230249, 0.620695, -0.749482));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.230249, 0.620695, -0.749482));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.230249, 0.620695, -0.749482));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.223943, 0.601371, -0.766944));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.223943, 0.601371, -0.766944));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.223943, 0.601371, -0.766944));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.222539, 0.574574, -0.787618));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.222539, 0.574574, -0.787618));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.222539, 0.574574, -0.787618));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.218895, 0.548605, -0.806919));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.218895, 0.548605, -0.806919));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.218895, 0.548605, -0.806919));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.218895, 0.548605, -0.806919));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.211309, 0.50564, -0.836467));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.211309, 0.50564, -0.836467));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.211309, 0.50564, -0.836467));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.145836, 0.52655, -0.837542));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.145836, 0.52655, -0.837542));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.145836, 0.52655, -0.837542));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.145836, 0.52655, -0.837542));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.142431, 0.557323, -0.817988));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.142431, 0.557323, -0.817988));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.142431, 0.557323, -0.817988));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.142431, 0.557323, -0.817988));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.138659, 0.588758, -0.796327));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.138659, 0.588758, -0.796327));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.138659, 0.588758, -0.796327));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.138659, 0.588758, -0.796327));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.129214, 0.63965, -0.757728));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.129214, 0.63965, -0.757728));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.129214, 0.63965, -0.757728));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.129214, 0.63965, -0.757728));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0976917, 0.594382, -0.798227));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0976917, 0.594382, -0.798227));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0976917, 0.594382, -0.798227));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0691766, 0.529512, -0.845477));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0691766, 0.529512, -0.845477));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0691766, 0.529512, -0.845477));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.111965, 0.561658, -0.819758));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.111965, 0.561658, -0.819758));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.111965, 0.561658, -0.819758));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.111965, 0.561658, -0.819758));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0899289, 0.568851, -0.817509));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0899289, 0.568851, -0.817509));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0899289, 0.568851, -0.817509));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0503732, 0.590167, -0.805708));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0503732, 0.590167, -0.805708));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0503732, 0.590167, -0.805708));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0503732, 0.590167, -0.805708));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0338687, 0.577425, -0.815741));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0338687, 0.577425, -0.815741));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0338687, 0.577425, -0.815741));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0185544, 0.554423, -0.832028));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0185544, 0.554423, -0.832028));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0185544, 0.554423, -0.832028));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0185544, 0.554423, -0.832028));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0185544, 0.554423, -0.832028));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.013507, 0.544223, -0.838832));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.013507, 0.544223, -0.838832));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.013507, 0.544223, -0.838832));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.013507, 0.544223, -0.838832));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0464458, 0.615211, -0.786993));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0464458, 0.615211, -0.786993));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0464458, 0.615211, -0.786993));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.0212219, 0.636965, -0.7706));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.021283, 0.634273, -0.772816));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.021283, 0.634273, -0.772816));
				if (!(rand() % 3)) stars.push_back(glm::vec3(-0.021283, 0.634273, -0.772816));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.000297912, 0.651635, -0.758533));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.000297912, 0.651635, -0.758533));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.000297912, 0.651635, -0.758533));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0330318, 0.660845, -0.749795));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0330318, 0.660845, -0.749795));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0330318, 0.660845, -0.749795));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0498802, 0.663459, -0.746549));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0498802, 0.663459, -0.746549));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0498802, 0.663459, -0.746549));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0769638, 0.666064, -0.741913));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0769638, 0.666064, -0.741913));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0769638, 0.666064, -0.741913));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.147258, 0.668661, -0.72884));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.147258, 0.668661, -0.72884));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.147258, 0.668661, -0.72884));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.102839, 0.643662, -0.758369));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.102839, 0.643662, -0.758369));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.115798, 0.599976, -0.791593));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.115798, 0.599976, -0.791593));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.115798, 0.599976, -0.791593));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.127866, 0.565979, -0.814444));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.127866, 0.565979, -0.814444));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.127866, 0.565979, -0.814444));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.11054, 0.666064, -0.737659));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.11054, 0.666064, -0.737659));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.11054, 0.666064, -0.737659));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.252911, 0.575999, -0.777342));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.252911, 0.575999, -0.777342));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.252911, 0.575999, -0.777342));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.266092, 0.594381, -0.758885));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.266092, 0.594381, -0.758885));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.266092, 0.594381, -0.758885));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.281528, 0.615211, -0.736381));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.281528, 0.615211, -0.736381));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.281528, 0.615211, -0.736381));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.249791, 0.632923, -0.732812));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.249791, 0.632923, -0.732812));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.249791, 0.632923, -0.732812));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.249791, 0.632923, -0.732812));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.248859, 0.65031, -0.71775));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.248859, 0.65031, -0.71775));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.248859, 0.65031, -0.71775));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.262302, 0.667363, -0.697011));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.262302, 0.667363, -0.697011));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.262302, 0.667363, -0.697011));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.286314, 0.67125, -0.683701));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.286314, 0.67125, -0.683701));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.286314, 0.67125, -0.683701));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.293335, 0.654276, -0.69705));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.293335, 0.654276, -0.69705));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.293335, 0.654276, -0.69705));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.3003, 0.632923, -0.713602));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.3003, 0.632923, -0.713602));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.3003, 0.632923, -0.713602));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.309495, 0.602763, -0.735452));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.309495, 0.602763, -0.735452));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.309495, 0.602763, -0.735452));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.313803, 0.583105, -0.749344));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.313803, 0.583105, -0.749344));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.313803, 0.583105, -0.749344));
				if (!(rand() % 3)) stars.push_back(glm::vec3(0.0673277, 0.176934, -0.981917));

				CGE::instance->vao->storeData(0, 3, &stars);
				glEnableVertexAttribArray(0);
				cnt = stars.size();
			}
			CGE::instance->camera->pos = tmp;

			CGE::instance->starShader->use();
			glm::mat4 view = CGE::instance->camera->createViewMatrix();
			float rot = float(CGE::instance->thePlayer->worldObj->time) / 48000.f * 360.f;
			for (int i = 0; i < 3; i++)
				view[3][i] = 0.f;
			view[3][3] = 1.f;
			CGE::instance->starShader->loadMatrix("mt", CGE::instance->projection * glm::rotate(view, glm::radians(rot), glm::vec3(1, 0, 0)));
			CGE::instance->starShader->loadFloat("str", glm::clamp(-sun.y * 10, 0.f, 1.f));
			glBindVertexArray(vao);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendEquation(GL_MAX);
			glDrawArrays(GL_POINTS, 0, cnt);
			glBlendEquation(GL_FUNC_ADD);
		}

		CGE::instance->gBuffer->flush(true);
		CGE::checkOpenGLError("Begin");
		CGE::instance->particleShader->start();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, CGE::instance->gBuffer->framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, CGE::instance->mainFB->framebuffer);
		glBlitFramebuffer(0, 0, CGE::instance->width, CGE::instance->height, 0, 0, CGE::instance->width, CGE::instance->height,
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glActiveTexture(GL_TEXTURE0);

		CGE::checkOpenGLError("Start");
		VAO vao;
		VAO::Loader l = vao;
		const float j = .3f;
		l.store(0, {
			{ -j, j, 0.0f, 1.f },
			{ -j, -j, 0.0f, 1.f },
			{ j, j, 0.0f, 1.f },
			{ j, -j, 0.0f, 1.f }
		});
		l.store(1, {
			{ 0, 1 },
			{ 0, 0 },
			{ 1, 1 },
			{ 1, 0 }
		});
		l.store(2, {
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 }
		});
		l.store({
			0, 1, 2, 2, 1, 3
		});
		std::vector<glm::vec3> trans;
		std::vector<glm::vec4> colors;
		std::vector<glm::vec4> uvs;
		size_t x = CGE::instance->thePlayer->worldObj->particles.size();
		trans.reserve(x);
		colors.reserve(x);
		for (std::deque<StandartFX*>::iterator i = CGE::instance->thePlayer->worldObj->particles.begin(); i != CGE::instance->thePlayer->worldObj->particles.end();)
		{
			(*i)->render();
			trans.push_back((*i)->mPos);
			colors.push_back((*i)->mColor);
			glm::vec2 uv1, uv2;
			CGE::instance->taParticles.generateUV((*i)->getTextureIndex(), uv1, uv2);
			uvs.push_back(glm::vec4(uv1, uv2));
			if ((*i)->lifetime >= (*i)->maxLifetime)
			{
				delete *i;
				i = CGE::instance->thePlayer->worldObj->particles.erase(i);
			}
			else
			{
				++i;
			}
		}
		l.store(3, trans, 1);
		l.store(4, colors, 1);
		l.store(5, uvs, 1);

		glm::mat4 mod;
		glm::mat4 view = CGE::instance->camera->createViewMatrix();
		mod[0][0] = view[0][0];
		mod[0][1] = view[1][0];
		mod[0][2] = view[2][0];
		mod[1][0] = view[0][1];
		mod[1][1] = view[1][1];
		mod[1][2] = view[2][1];
		mod[2][0] = view[0][2];
		mod[2][1] = view[1][2];
		mod[2][2] = view[2][2];
		glm::mat4 vp = CGE::instance->projection * view;
		CGE::instance->particleShader->loadMatrix("VP", vp);
		CGE::instance->particleShader->loadMatrix("M", mod);
		static GLuint tex = CGE::instance->textureManager->loadTexture("ta/particles", true);
		glBindTexture(GL_TEXTURE_2D, tex);

		vao.drawInstanced(GL_TRIANGLES, CGE::instance->thePlayer->worldObj->particles.size());


		CGE::instance->particleShader2->start();
		for (std::deque<CustomFX*>::iterator i = CGE::instance->thePlayer->worldObj->customParticles.begin(); i != CGE::instance->thePlayer->worldObj->customParticles.end();)
		{
			CustomFX* p = *i;
			if (!p->shader)
			{
				if (const char* code = p->getCustomShaderCode())
				{
					boost::shared_ptr<Shader> shader;
					if (!(shader = CGE::instance->customShaders[std::string("csp_") + typeid(p).name()]).get())
					{

						std::map<std::string, std::string> params;
						p->getCustomShaderParams(params);
						static std::string vertex;
						if (vertex.empty())
						{
							Utils::fileToString("res/glsl/particle2V.glsl", vertex);
						}
						std::string fragment = "#version 330 core\n"
							"in vec4 color_pass;\n"
							"in vec2 uv_pass;\n"
							"out vec4 color;\n"
							"uniform vec4 c;\n";
						for (std::pair<std::string, std::string> p : params)
						{
							fragment += std::string("uniform ") + p.second + " " + p.first + ";\n";
						}
						fragment += "vec4 user_code() {";
						fragment += code;
						fragment += "}\n"
							"void main(void){"
							"color = user_code() * color_pass * c;"
							"}"
							;

						shader = boost::make_shared<CustomShader>(vertex, fragment);
						shader->compile();
						shader->bindAttribute("position");
						shader->bindAttribute("uv");
						shader->bindAttribute("color");
						shader->link();
						shader->start();
						shader->loadInt("tex", 0);
						shader->loadVector("c", glm::vec4(1.f));
					}
					CGE::instance->customShaders[std::string("csp_") + typeid(p).name()] = shader;
					p->shader = shader.get();
				} else
				{
					p->shader = CGE::instance->particleShader2;
				}
			}
			if (p->shader != CGE::instance->currentShader)
			{
				p->shader->start();
				CGE::instance->particleShader2->loadMatrix("VP", vp);
			}
			p->render();
			if (p->lifetime >= p->maxLifetime)
			{
				delete p;
				i = CGE::instance->thePlayer->worldObj->customParticles.erase(i);
			}
			else
			{
				++i;
			}
		}
		glEnable(GL_BLEND);
		CGE::checkOpenGLError("Post");

		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
		
		CGE::instance->defaultStaticShader->start();
		CGE::instance->renderer->setColor(glm::vec4(1.f));
		CGE::instance->setDebugString("Particles", std::to_string(CGE::instance->thePlayer->worldObj->particles.size()));
		CGE::checkOpenGLError("End.");
	}
}

#endif
