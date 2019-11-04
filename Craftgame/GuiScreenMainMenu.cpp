#include "GuiScreenMessageDialog.h"
#include "GuiScreenMultiplayer.h"
#include "GuiScreenSingleplayer.h"
#ifndef SERVER
#include "GuiScreenScreenshots.h"
#include "GuiScreenMainMenu.h"
#include "GuiButton.h"
#include "GameEngine.h"
#include "Dir.h"
#include <algorithm>
#include <random>
#include "Models.h"
#include "global.h"
#include <iomanip>
#include <glm/gtc/matrix_transform.hpp>
#include "GuiScreenSettings.h"
#include "OS.h"
#include "build.h"

GuiScreenMainMenu::GuiScreenMainMenu()
{

	addWidget(new GuiButton(_("gui.mm.singleplayer"), [](int, int, int) {
		CGE::instance->displayGuiScreen(new GuiScreenSingleplayer);
	}));
	addWidget(new GuiButton(_("gui.mm.multiplayer"), [](int, int, int) {
		CGE::instance->displayGuiScreen(new GuiScreenMultiplayer);
	}));
	addWidget(new GuiButton(_("gui.mm.settings"), [](int, int, int) {
		CGE::instance->displayGuiScreen(new GuiScreenSettings);
	}));
	addWidget(new GuiButton(_("gui.mm.exit"), [](int, int, int) {
#ifdef WINDOWS
		PostQuitMessage(0);
#else
		exit(0);
#endif
	}));
	/*
	seek->minValue = 0;
	seek->onValueChangingStop = [](int v) {
		CGE::instance->settings->setProperty(_R("craftgame:render/distance"), v);
		CGE::instance->settings->apply();
	};
	seek->value = CGE::instance->settings->getProperty(_R("craftgame:render/distance"), 2);
	widgets.push_back(seek);/*
	widgets.push_back(new GuiSeekBar(20, 100, 400, 40, [](int value) {
		static Material* gold = CGE::instance->materialRegistry->getMaterial(_R("craftgame:custom"));
		float ref = float(value) / 100.f;
		gold->reflectivity = ref;
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << ref;
		return std::string("Reflectivity: " + ss.str());
	}, 1000));
	widgets.push_back(new GuiSeekBar(20, 140, 400, 40, [](int value) {
		static Material* gold = CGE::instance->materialRegistry->getMaterial(_R("craftgame:custom"));
		float shineDump = float(value) / 100.f;
		gold->shineDump = shineDump;
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << shineDump;
		return std::string("Shine dump: " + ss.str());
	}, 1000));*/
	l = new LightSource(glm::vec3(-10, 0, 0), glm::vec3(4.f), 150.f);
}

GuiScreenMainMenu::~GuiScreenMainMenu() {
	delete l;
}
void GuiScreenMainMenu::onLayoutChange(int x, int y, int width, int height) {
	GuiScreen::onLayoutChange(x, y, width, height);
	widgets[0]->onLayoutChange(20, 140, 400, 40);
	widgets[1]->onLayoutChange(20, 200, 400, 40);
	widgets[2]->onLayoutChange(20, 260, 400, 40);
	widgets[3]->onLayoutChange(20, 320, 400, 40);
}
float z;
void GuiScreenMainMenu::render(int rf, int mouseX, int mouseY) {
	static AdvancedModel* cube = CGE::instance->modelRegistry->getModel(_R("craftgame:cube"));
	static Material* surf = CGE::instance->materialRegistry->getMaterial(_R("craftgame:surface"));
	GuiScreen::render(rf, mouseX, mouseY);
	CGE::instance->renderer->renderFlags &= ~GameRenderer::RF_SSAO;
	CGE::instance->defaultStaticShader->start();
	CGE::instance->gBuffer->begin();
	static float oldX = float(mouseX) / float(width);
	static float oldY = float(mouseY) / float(height);
	float newX = float(mouseX) / float(CGE::instance->width);
	float newY = float(mouseY) / float(CGE::instance->height);
	newX = oldX = glm::mix(oldX, newX, 0.01f);
	newY = oldY = glm::mix(oldY, newY, 0.01f);
	CGE::instance->renderer->setColor(getColor());
	CGE::instance->cursor = !dbg;
#define SPEED 50.f
	if (dbg) {
		if (Keyboard::isKeyDown(Keyboard::W)) {
			posZ -= CGE::instance->millis * SPEED * sin((-pitch + 90.f) * M_PI / 180.f);
			posX -= CGE::instance->millis * SPEED * cos((-pitch + 90.f) * M_PI / 180.f);
		}
		if (Keyboard::isKeyDown(Keyboard::S)) {
			posZ -= CGE::instance->millis * SPEED * sin((-pitch - 90.f) * M_PI / 180.f);
			posX -= CGE::instance->millis * SPEED * cos((-pitch - 90.f) * M_PI / 180.f);
		}
		if (Keyboard::isKeyDown(Keyboard::A)) {
			posZ -= CGE::instance->millis * SPEED * sin((-pitch) * M_PI / 180.f);
			posX -= CGE::instance->millis * SPEED * cos((-pitch) * M_PI / 180.f);
		}
		if (Keyboard::isKeyDown(Keyboard::D)) {
			posZ -= CGE::instance->millis * SPEED * sin((-pitch + 180.f) * M_PI / 180.f);
			posX -= CGE::instance->millis * SPEED * cos((-pitch + 180.f) * M_PI / 180.f);
		}
		if (Keyboard::isKeyDown(Keyboard::Space)) {
			posY += CGE::instance->millis * SPEED;
		}
		if (Keyboard::isKeyDown(Keyboard::LShift)) {
			posY -= CGE::instance->millis * SPEED;
		}
		float xc = float(width) / 2.f;
		float yc = float(height) / 2.f;
		pitch += (xc - mouseX) * 0.1f;
		yaw += (yc - mouseY) * 0.1f;
		OS::setCursorPos(int(xc), int(yc));
		CGE::instance->camera->pos.x = posX;
		CGE::instance->camera->pos.y = posY;
		CGE::instance->camera->pos.z = posZ;
		yaw = glm::clamp(yaw, -90.f, 90.f);
		CGE::instance->camera->yaw = -pitch;
		CGE::instance->camera->pitch = -yaw;
	}
	else {
		float rot = glm::clamp(newX, 0.f, 1.f) - 1;
		float rotY = glm::clamp(newY, 0.f, 1.f) - 0.5f;
		CGE::instance->camera->pos.x = sin(rot) * 0.002f - 31.08f;
		CGE::instance->camera->pos.y = -sin(rotY) * 0.001f + 51.7f;
		CGE::instance->camera->pos.z = cos(rot) * 0.002f - 36.6f;
		CGE::instance->camera->yaw = -248.7f;
		CGE::instance->camera->pitch = 8.2f;
	}
	l->pos = CGE::instance->camera->pos;
	glm::mat4 ma = getTransform();
	glEnable(GL_DEPTH_TEST);
	CGE::instance->renderer->setTexturingMode(0);
	glm::mat4 m = glm::mat4(1.0);
	m = glm::translate(m, glm::vec3(-10, 50, 0));
	m = glm::scale(m, -glm::vec3(100.f));
	CGE::instance->renderer->transform = m * ma;
	CGE::instance->defaultStaticShader->loadMatrix("transform", CGE::instance->renderer->transform);
	surf->bind();
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(0.4, 0.4, 0.4, 1));
	CGE::instance->renderer->renderModel(cube, false);
	m = glm::mat4(1.0);
	m = glm::scale(m, glm::vec3(0.34f));
	CGE::instance->renderer->transform = m;
	CGE::instance->defaultStaticShader->loadMatrix("transform", CGE::instance->renderer->transform);
	//CGE::instance->camera->pos.z = l->pos.z - 2 * sin((-pitch + 180) * M_PI / 180.f);
	//CGE::instance->camera->pos.x = l->pos.x - 2 * cos((-pitch + 180) * M_PI / 180.f);
	//CGE::instance->camera->upload();
	//glColorMask(true, false, false, true);
	//player->render(); 
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glColorMask(false, true, true, true);
	//CGE::instance->camera->pos.z = l->pos.z - 2 * sin((-pitch) * M_PI / 180.f);
	//CGE::instance->camera->pos.x = l->pos.x - 2 * cos((-pitch) * M_PI / 180.f);
	//CGE::instance->camera->upload();
	//player->render();
	//glColorMask(true, true, true, true);
	glDisable(GL_DEPTH_TEST);
	CGE::instance->camera->reset();
	CGE::instance->camera->upload();
	CGE::instance->gBuffer->flush(false);
	CGE::instance->guiShader->start();
	CGE::instance->renderer->setTransform(glm::mat4(1.0));
	CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 1));
	CGE::instance->renderer->drawString(4, CGE::instance->height - 32, std::string(VERSION) + " build " + std::to_string(CGE_BUILD) + "\n" + __DATE__ + " " + __TIME__, Align::LEFT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.4) * getColor());
	{
		char buf[64];
		std::time_t t = time(0);
		std::strftime(buf, sizeof(buf), "%H:%M", localtime(&t));
		CGE::instance->renderer->drawString(width - 4, 4, buf, Align::RIGHT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.3) * getColor());
		CGE::instance->renderer->drawString(width - 50, 4, CGE::instance->gameProfile->getUsername(), Align::RIGHT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.3) * getColor());
	}
	CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 1));
	if (dbg)
		CGE::instance->renderer->drawString(CGE::instance->width - 4, 4, "FREE CAMERA MODE", Align::RIGHT, TextStyle::SIMPLE, glm::vec4(1, 0, 0, 0.4));
	//CGE::instance->renderer->drawString(CGE::instance->width - 4, CGE::instance->height - 20, "(c) Alex2772 2k17", Align::RIGHT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.4));
	CGE::instance->renderer->renderFlags |= GameRenderer::RF_SSAO;
}
void GuiScreenMainMenu::onKeyDown(Keyboard::Key key, byte data) {
	GuiScreen::onKeyDown(key, data);
	switch (key) {
	case Keyboard::LControl:
		dbg = !dbg;
		l->color = glm::vec3(4.f);
		break;
	/*case Keyboard::S:
		CGE::instance->displayGuiScreen(new GuiScreenScreenshots());
		break;*/
	}
}
void GuiScreenMainMenu::renderWidgets(int rf, int mouseX, int mouseY)
{
	if (!dbg)
		GuiScreen::renderWidgets(rf, mouseX, mouseY);
}
#endif
