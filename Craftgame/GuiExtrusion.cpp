#ifndef SERVER
#include "GuiExtrusion.h"
#include "GameEngine.h"

GuiExtrusion::GuiExtrusion(): atlas(CGE::instance->textureManager->loadTexture("res/gui/inv.png", false), 3, 3)
{
}

void GuiExtrusion::render(int flags, int mouseX, int mouseY)
{
	Gui::render(flags, mouseX, mouseY);
	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->setColor(glm::vec4(1));

	atlas.bindTexture(top_left);
	CGE::instance->renderer->drawTextureAtlas(0, 0, 8, 8);

	atlas.bindTexture(top);
	CGE::instance->renderer->drawTextureAtlas(8, 0, width - 16, 8);

	atlas.bindTexture(top_right);
	CGE::instance->renderer->drawTextureAtlas(width - 8, 0, 8, 8);

	atlas.bindTexture(left);
	CGE::instance->renderer->drawTextureAtlas(0, 8, 8, height - 16);

	atlas.bindTexture(center);
	CGE::instance->renderer->drawTextureAtlas(8, 8, width - 16, height - 16);

	atlas.bindTexture(right);
	CGE::instance->renderer->drawTextureAtlas(width - 8, 8, 8, height - 16);

	atlas.bindTexture(bottom_left);
	CGE::instance->renderer->drawTextureAtlas(0, height - 8, 8, 8);

	atlas.bindTexture(bottom);
	CGE::instance->renderer->drawTextureAtlas(8, height - 8, width - 16, 8);

	atlas.bindTexture(bottom_right);
	CGE::instance->renderer->drawTextureAtlas(width - 8, height - 8, 8, 8);
}
#endif