#ifndef SERVER
#include "GuiScreenParent.h"
#include "GameEngine.h"

GuiScreenParent::GuiScreenParent(GuiScreen* _parent) :
	parent(_parent) {

}
void GuiScreenParent::render(int rf, int mouseX, int mouseY) {
	GuiScreen::render(rf, mouseX, mouseY);
	parent->render(rf | RenderFlags::OVERLAYED | RenderFlags::NO_BLUR, mouseX, mouseY);
	parent->renderWidgets(rf | RenderFlags::OVERLAYED | RenderFlags::NO_BLUR, mouseX, mouseY);
	if (!(rf & RenderFlags::OVERLAYED)) {
		CGE::instance->renderer->blur(0, 0, CGE::instance->width, CGE::instance->height);
		CGE::instance->guiShader->loadFloat("texturing", 0.f);
		CGE::instance->renderer->setColor(glm::vec4(0.1, 0.1, 0.1, 0.7));
		CGE::instance->renderer->drawRect(0, 0, CGE::instance->width, CGE::instance->height);
		CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 1));
		CGE::instance->guiShader->loadFloat("texturing", 1.f);
	}
}
/*
void GuiScreenParent::renderWidgets(int _rf, int mouseX, int mouseY) {
	int mX = CGE::instance->mouseX;
	int mY = CGE::instance->mouseY;
	for (int i = 0; i < widgets.size(); i++) {
		int rf = _rf;
		if (!useBlur)
			rf |= RenderFlags::NO_BLUR;
		if (!(rf & RenderFlags::OVERLAYED) && widgets[i]->x <= mX && widgets[i]->x + widgets[i]->width >= mX && widgets[i]->y <= mY && widgets[i]->y + widgets[i]->height >= mY)
			rf |= RenderFlags::MOUSE_HOVER;
		widgets[i]->render(rf, widgets[i]->x - mouseX, widgets[i]->y - mouseY);
	}
}*/
void GuiScreenParent::onLayoutChange(int x, int y, int width, int height) {
	GuiScreen::onLayoutChange(x, y, width, height);
	parent->onLayoutChange(x, y, width, height);
}

#endif
