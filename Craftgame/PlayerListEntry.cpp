#include "GuiTooltip.h"
#ifndef SERVER
#include "PlayerListEntry.h"
#include "GameEngine.h"
#include <chrono>

PlayerListEntry::PlayerListEntry(id_type id, const std::string& name, short ping)
	: id(id),
	name(name),
	ping(ping),
	icons(0, 4, 1)
{
	CGE::instance->uiThread.push([&]()
	{
		icons.texture = CGE::instance->textureManager->loadTexture(_R("craftgame:res/icon/player.png"));
	});
	tooltip = new GuiTooltip;
}

void PlayerListEntry::renderSignal(unsigned char count, int x, int y)
{
	CGE::instance->renderer->setTexturingMode(0);
	for (unsigned char i = 0; i < count; i++)
	{
		CGE::instance->renderer->drawRect(x + i * 8, y - i * 4, 4, i * 4);
	}
}

void PlayerListEntry::render(int flags, int mouseX, int mouseY)
{
	if (mouseX > width - 45)
	{
		dynamic_cast<GuiTooltip*>(tooltip)->setText(std::to_string(ping) + " ms");
	} else
	{
		dynamic_cast<GuiTooltip*>(tooltip)->setText("");
	}
	for (size_t i = 0; i < 3; i++) {
		if (1 << i & state) {
			if (mouseX > width - 92 - i * 35 && mouseX < width - 92 - (i - 1) * 35) {
				std::vector<std::string> tlp = {
					("gui.pause.player.afk"),
					("gui.pause.player.writing_message"),
					("gui.pause.player.alttab")
				};
				dynamic_cast<GuiTooltip*>(tooltip)->setText(_(tlp[i]));
			}
		}
	}
	Gui::render(flags, mouseX, mouseY);
	CGE::instance->renderer->drawString(2, 2, name, Align::LEFT, TextStyle::SIMPLE, glm::vec4(1.f), 20);
	CGE::instance->renderer->setColor(glm::vec4(0, 0.1f, 0, 1.f));
	renderSignal(5, width - 45, height - 10);
	CGE::instance->renderer->setColor(glm::vec4(0, 1, 0, 1.f));
	unsigned char c = 0;
	if (ping < 200)
	{
		c = 5;
	} else if (ping < 400)
	{
		c = 4;
	}
	else if (ping < 600)
	{
		c = 3;
	}
	else if (ping < 800)
	{
		c = 2;
	}
	else if (ping < 1000)
	{
		c = 1;
	}
	renderSignal(c, width - 47, height - 12);
	float blink = float(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() % 1000) / 1000.f;
	float alpha = (cos(blink * 20 / 3.14) + 1.f) / 2.f;
	for (size_t i = 0; i < 3; i++) {
		if (1 << i & state) {
			if (i == 1)
			{
				CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, alpha));
			}
			else
			{
				CGE::instance->renderer->setColor(glm::vec4(1));
			}
			icons.bindTexture(i);
			CGE::instance->renderer->drawTextureAtlas(width - 92 - i * 35, 4, 32, 32);
		}
	}
}
#endif