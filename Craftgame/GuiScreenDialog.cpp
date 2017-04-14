#ifndef SERVER
#include "GuiScreenDialog.h"
#include "GameEngine.h"
#include "AnimationDialog.h"


GuiScreenDialog::GuiScreenDialog(Animation* _a):
	animation(_a)
{
	animations.push_back(animation);
}
void GuiScreenDialog::render(int rf, int mouseX, int mouseY) {
	static size_t c = CGE::instance->guiShader->getUniform("c");
	static size_t texturing = CGE::instance->guiShader->getUniform("texturing");
	GuiScreen::render(rf, mouseX, mouseY);
	glm::vec4 color = getColor();
	CGE::instance->guiShader->loadInt(texturing, 0);
	CGE::instance->guiShader->loadVector(c, glm::vec4(0, 0, 0, 0.4f) * color);
	glm::mat4 m2 = CGE::instance->renderer->transform;
	CGE::instance->renderer->transform = glm::mat4(1.0);
	CGE::instance->guiShader->loadInt("masking", 0);
	CGE::instance->renderer->drawRect(0, 0, CGE::instance->width, CGE::instance->height);
	CGE::instance->guiShader->loadInt("masking", 1);
	CGE::instance->renderer->transform = m2;
	CGE::instance->renderer->blur(0, 0, width, height);
	CGE::instance->guiShader->loadVector(c, glm::vec4(0.2, 0.2, 0.2, 0.4f) * color);
	CGE::instance->renderer->drawRect(0, 0, width, 80);
	CGE::instance->renderer->drawRect(0, 80, width, height - 80);
	CGE::instance->guiShader->loadInt(texturing, 1);
	CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 1));
}
void GuiScreenDialog::onLayoutChange(int _x, int _y, int _width, int _height) {
	width = CGE::instance->width * 0.8f;
	height = CGE::instance->height * 0.7f;
	x = CGE::instance->width * 0.1f;
	y = CGE::instance->height * 0.15f;
}
void GuiScreenDialog::close() {
	if (animation)
		GuiScreen::animatedClose(animation);
	else
		GuiScreen::close();
}
#endif