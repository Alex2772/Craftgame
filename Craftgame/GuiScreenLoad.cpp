#include "Dir.h"
#ifndef SERVER
#include "GuiScreenLoad.h"
#include "GameEngine.h"
#include "Models.h"
#include "Blocks.h"

void GuiScreenLoad::onLayoutChange(int _x, int _y, int _width, int _height)
{
	GuiScreen::onLayoutChange(_x, _y, _width, _height);
	if (!fb)
		fb = new Framebuffer(_width, _height);
}

GuiScreenLoad::GuiScreenLoad()
{
	b = new BlockGrass;
	CGE::instance->camera->pos.x = CGE::instance->camera->pos.y = CGE::instance->camera->pos.z = 0;
	CGE::instance->camera->yaw = 0;
	CGE::instance->camera->pitch = 0;
	l = new LightSource(glm::vec3(-6, 8, 5), glm::vec3(6, 6, 6), 30.f);
	l2 = new LightSource(glm::vec3(-1, 1, 0), glm::vec3(4.5, 4.5, 4.5));
}

GuiScreenLoad::~GuiScreenLoad()
{
	if (l) {
		delete l;
		delete l2;
	}
	if (fb)
		delete fb;
	delete b;
}

void GuiScreenLoad::animatedClose(Animation * a)
{
	GuiScreen::animatedClose(a);
	delete l;
	delete l2;
	l = l2 = nullptr;
}

void GuiScreenLoad::render(int _rf, int mouseX, int mouseY)
{
	GuiScreen::render(_rf, mouseX, mouseY);
	if (l2) {
		Framebuffer* temp = CGE::instance->mainFB;
		CGE::instance->mainFB = fb;
		fb->bind(width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 1));
		CGE::instance->renderer->setTexturingMode(0);
		CGE::instance->renderer->drawRect(0, 0, width, height);
		CGE::instance->renderer->setTexturingMode(1);
		glm::mat4 ma = glm::mat4(1.0);
		a += CGE::instance->millis * 60.f;
		if (a > 1000.f)
			a -= 1000.f;

		glm::mat4 m_light = glm::rotate(glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)), glm::radians(a / 2.77f), glm::vec3(0.5f, 0.3f, 0.4f));
		m_light = glm::translate(m_light, glm::vec3(0, 0, -3.f));
		l2->pos = glm::vec3(m_light * glm::vec4(1, 1, 1, 1));

		glEnable(GL_DEPTH_TEST);
		glm::mat4 m = glm::mat4(1.0);

		m = glm::translate(m, glm::vec3(0, -.1f, -.5f));
		glEnable(GL_CULL_FACE);
		CGE::instance->gBuffer->begin();
		CGE::instance->defaultStaticShader->start();
		CGE::instance->renderer->transform = glm::scale(m * ma, glm::vec3(2.f));
		CGE::instance->renderer->renderBlockIcon(b);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		CGE::instance->gBuffer->flush(true);
		CGE::instance->guiShader->start();
		CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 1));
		CGE::instance->mainFB = temp;
		CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	}
	static GLuint transform = CGE::instance->guiShader->getUniform("transform");
	static GLuint c = CGE::instance->guiShader->getUniform("c");
	static GLuint uv = CGE::instance->guiShader->getUniform("uv");
	static GLuint uv2 = CGE::instance->guiShader->getUniform("uv2");
	CGE::instance->renderer->setTexturingMode(1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fb->texture);
	CGE::instance->guiShader->loadVector(c, getColor());
	CGE::instance->guiShader->loadVector(uv, glm::vec2(0, 0));
	CGE::instance->guiShader->loadVector(uv2, glm::vec2(1, 1));
	CGE::instance->guiShader->loadMatrix(transform, getTransform());
	CGE::instance->renderer->renderModel(Models::viewport);
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	CGE::instance->renderer->setTransform(glm::mat4(1.0));

	if (animations.empty()) {
		glBindTexture(GL_TEXTURE_2D, CGE::instance->textureManager->loadTexture("res/gui/spin.png", true));
		CGE::instance->renderer->setTexturingMode(1);
		static float sp = 0.f;
		glm::mat4 ortho = glm::ortho(width / -2.f, width / 2.f, height / -2.f, height / 2.f, 0.1f, 10.f);
		CGE::instance->renderer->setTransform(ortho * glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(width / 2.f - 28, -(height / 2.f - 28), -1.f)), glm::radians(-sp), glm::vec3(0, 0, 1.f)));
		sp += CGE::instance->millis * 180.f;
		sp = fmodf(sp, 360.f);
		CGE::instance->renderer->noAT = true;
		CGE::instance->renderer->drawRect(-14, -14, 28, 28);
		CGE::instance->renderer->noAT = false;
	}
	CGE::instance->renderer->setTransform(glm::mat4(1.0));

	if (!displayString.empty())
	{
		CGE::instance->renderer->drawString(width / 2, height / 4 * 3, displayString, Align::CENTER);
	}
}

void GuiScreenLoad::setDisplayStringAsync(std::string s)
{
	INFO(s);
	displayString = s;
}
#endif
