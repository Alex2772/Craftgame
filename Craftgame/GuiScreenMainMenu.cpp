#ifndef SERVER
#include "GuiScreenMainMenu.h"
#include "GuiButton.h"
#include "GuiSeekBar.h"
#include "GameEngine.h"
#include "Dir.h"
#include <algorithm>
#include <random>
#include "Models.h"
#include "global.h"
#include <iomanip>
#include <glm/gtc/matrix_transform.hpp>
#include "GuiScreenSettings.h"
#include "GuiScreenMultiplayer.h"

GuiScreenMainMenu::GuiScreenMainMenu() {
	GuiScreenMainMenu* th = this;
	addWidget(new GuiButton("Singleplayer", [](int, int, int) {
		CGE::instance->logger->info("228");
	}));
	addWidget(new GuiButton("Multiplayer", [th](int, int, int) {
		CGE::instance->displayGuiScreen(new GuiScreenMultiplayer);
	}));
	addWidget(new GuiButton("Settings", [th](int, int, int) {
		CGE::instance->displayGuiScreen(new GuiScreenSettings);
	}));
	addWidget(new GuiButton("Exit", [](int, int, int) {
		PostQuitMessage(0);
	}));
	/*
	GuiSeekBar* seek = new GuiSeekBar(20, 60, 800, 40, [&](int value) {
		depth = value;
		return std::string("x: ") + std::to_string(depth);
	}, 100);
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
	CGE::instance->light.push_back(l = new LightSource(glm::vec3(1, 1, 1), glm::vec3(0.3, 0.3, 0.3)));
}

GuiScreenMainMenu::~GuiScreenMainMenu() {
	CGE::instance->light.erase(std::find(CGE::instance->light.begin(), CGE::instance->light.end(), l));
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
	static AdvancedModel* model = CGE::instance->modelRegistry->getModel(_R("craftgame:menu/main"));
	static AdvancedModel* cube = CGE::instance->modelRegistry->getModel(_R("craftgame:cube"));
	static Material* mat = CGE::instance->materialRegistry->getMaterial(_R("craftgame:block/golden"));
	static Material* surf = CGE::instance->materialRegistry->getMaterial(_R("craftgame:surface"));
	static size_t* skin = CGE::instance->skinManager.getSkin(CGE::instance->gameProfile->getUsername());
	GuiScreen::render(rf, mouseX, mouseY);
	CGE::instance->staticShader->start();
	static size_t t = CGE::instance->textureManager->loadTexture("res/blocks/bedrock.png");
	glBindTexture(GL_TEXTURE_2D, t);
	static float oldX = float(mouseX) / float(width);
	static float oldY = float(mouseY) / float(height);
	float newX = float(mouseX) / float(CGE::instance->width);
	float newY = float(mouseY) / float(CGE::instance->height);
	newX = oldX = glm::mix(oldX, newX, 0.01f);
	newY = oldY = glm::mix(oldY, newY, 0.01f);
	CGE::instance->staticShader->loadVector("c", getColor());
	
	glm::mat4 ma = getTransform();

	float rot = glm::clamp(newX, 0.f, 1.f) - 1;
	float rotY = glm::clamp(newY, 0.f, 1.f) - 0.5f;
	CGE::instance->camera->pos.x = sin(rot) * 0.002f;
	CGE::instance->camera->pos.y = -sin(rotY) * 0.001f;
	CGE::instance->camera->pos.z = cos(rot) * 0.002f;
	CGE::instance->camera->pos.z += z;
	CGE::instance->camera->yaw = -rot * 5 - 7.f;
	CGE::instance->camera->pitch = -rotY * 5 + 7.f;
	glEnable(GL_DEPTH_TEST);
	CGE::instance->staticShader->loadInt("texturing", 0);
	glm::mat4 m = glm::mat4(1.0);
	m = glm::translate(m, glm::vec3(0, 94, 0));
	m = glm::scale(m, glm::vec3(100));
	CGE::instance->staticShader->loadMatrix("transform", m * ma);
	glCullFace(GL_FRONT);
	surf->bind();
	CGE::instance->renderer->renderModel(cube, false);
	glCullFace(GL_BACK);
	CGE::instance->staticShader->loadInt("texturing", 1);
	m = glm::mat4(1.0);

	m = glm::translate(m, glm::vec3(0, -1, 0));
	m = glm::scale(m, glm::vec3(0.01f));

	m = glm::scale(glm::mat4(1.0), glm::vec3(0.11));
	m = glm::translate(m, glm::vec3(18, -54, -44));
	m = glm::rotate(m, glm::radians(180.f), glm::vec3(0, 1, 0));
	mat->bind();
	CGE::instance->staticShader->loadMatrix("transform", m * ma);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *skin);
	glDisable(GL_CULL_FACE);
	CGE::instance->renderer->renderModel(model, false);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	CGE::instance->guiShader->start();
	CGE::instance->renderer->drawString(4, CGE::instance->height - 20, VERSION, Align::LEFT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.4) * getColor());
	CGE::instance->guiShader->loadVector("c", glm::vec4(1, 1, 1, 1));
	//CGE::instance->renderer->drawString(CGE::instance->width - 4, CGE::instance->height - 20, "(c) Alex2772 2k17", Align::RIGHT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.4));
}
#endif