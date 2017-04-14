#ifndef SERVER
#include "GuiConnectingPanel.h"
#include "GameEngine.h"
#include "Utils.h"
#include "GuiButton.h"
#include "AnimationFade.h"

#define VAPE
#ifdef VAPE
static size_t textures[190];
#endif
static byte loaded = 0;
static byte state = 1;


class GCP_Anim: public Animation {
public:
	GCP_Anim(Gui* g):
		Animation(g, 6)
	{
		frame = 6;
	}
	virtual void process() {
		Animation::process();
		gui->transform = glm::translate(glm::mat4(1.0), glm::vec3((frame / 6.f) * (frame / 6.f) * 2, 0, 0));
	}
};

GuiConnectingPanel::GuiConnectingPanel() {
	if (loaded == 0) {
		loaded = 1;
#ifdef VAPE
		CGE::instance->threadPool.runAsync([&]() {
			for (byte i = 0; i < 190; i++) {
				state = i;
				std::string s2 = std::to_string(i);
				std::string s;
				for (byte i = 0; i < 4 - s2.size(); i++)
					s += "0";
				s += s2;
				Image* img = CGE::instance->imageLoader->loadPNG(std::string("res/sequence/vape/Kek") + s + ".png");
				CGE::instance->secondThread.push([&, i, img, s]() {
					textures[i] = CGE::instance->textureManager->loadTexture(img, std::string("res/sequence/vape/Kek") + s + ".png", true);
					if (int(i) == 189)
						loaded = 2;
				});
			}
		});
#endif
	}
	addWidget(new GuiButton("X", [&](int, int, int) {
		collapse();
	}));
	widgets[0]->color = Utils::fromHex(0xff2222c0);
	resetTimer();
	animations.push_back(animation = new GCP_Anim(this));
}
void GuiConnectingPanel::collapse() {
	animation->state = State::play;
}
void GuiConnectingPanel::show() {
	animation->state = State::inverse;
}
void GuiConnectingPanel::onLayoutChange(int x, int y, int width, int height) {
	GuiContainer::onLayoutChange(x, y, width, height);
	widgets[0]->onLayoutChange(655, 232, 40, 40);
}
void GuiConnectingPanel::resetTimer() {
	timer = std::time(nullptr);
}
void GuiConnectingPanel::render(int rf, int mouseX, int mouseY) {
	static Font* futura = CGE::instance->fontRegistry->getFont(_R("craftgame:futura"));
	static Font* light = CGE::instance->fontRegistry->getFont(_R("craftgame:futuralight"));
	static float sa = 0;
	GuiContainer::render(rf, mouseX, mouseY);
	CGE::instance->guiShader->loadVector("c", glm::vec4(2, 2, 2, 2.5f) * getColor());
#ifdef VAPE
	if (loaded == 2 && animation->frame == 0) {
		CGE::instance->guiShader->loadInt("texturing", 1);
		animTexture += CGE::instance->millis * 35;
		if (animTexture > 189.f)
			animTexture -= 129.f;
		if (animTexture > 169.f) {
			float a = 189.f - animTexture;
			a /= 20.f;
			CGE::instance->guiShader->loadVector("c", glm::vec4(2, 2, 2, (1.f - a) * 2.5f) * getColor());
			glBindTexture(GL_TEXTURE_2D, textures[(int)round(animTexture - 129.f)]);
			CGE::instance->renderer->drawRect(0, 0, width, height);
			CGE::instance->guiShader->loadVector("c", glm::vec4(2, 2, 2, a * 2.5f) * getColor());
		}
		glBindTexture(GL_TEXTURE_2D, textures[(int)round(animTexture)]);
		CGE::instance->renderer->drawRect(0, 0, width, height);
	}
	else {
		animTexture = 0;
	}
#endif
	CGE::instance->guiShader->loadVector("c", getColor());
	int x2 = 250, y2 = 200;
	CGE::instance->guiShader->loadInt("texturing", 3);

	if (displayTop) {
		CGE::instance->guiShader->loadArray("colors", std::vector<glm::vec4>({ glm::vec4(0, 0, 0, 0), Utils::fromHex(0x478eaba0), glm::vec4(0, 0, 0, 0), Utils::fromHex(0x478eaba0) }));
		CGE::instance->renderer->drawRect(x2, y2, 530, 1);

		CGE::instance->guiShader->loadArray("colors", std::vector<glm::vec4>({ glm::vec4(0, 0, 0, 0), Utils::fromHex(0x32657ad0), glm::vec4(0, 0, 0, 0), Utils::fromHex(0x32657ad0) }));
		CGE::instance->renderer->drawRect(x2, y2 + 1, 530, 25);
	}
	CGE::instance->guiShader->loadArray("colors", std::vector<glm::vec4>({ glm::vec4(0, 0, 0, 0), Utils::fromHex(0x478eabfe), glm::vec4(0, 0, 0, 0), Utils::fromHex(0x478eabfe) }));
	CGE::instance->renderer->drawRect(x2, y2 + 26, 530, 1);
	CGE::instance->guiShader->loadArray("colors", std::vector<glm::vec4>({ glm::vec4(0, 0, 0, 0), Utils::fromHex(0x478eabce), glm::vec4(0, 0, 0, 0), Utils::fromHex(0x478eabce) }));
	CGE::instance->renderer->drawRect(x2, y2 + 75, 530, 1);

	CGE::instance->guiShader->loadArray("colors", std::vector<glm::vec4>({ glm::vec4(0, 0, 0, 0), Utils::fromHex(0x32657afe), glm::vec4(0, 0, 0, 0), Utils::fromHex(0x32657afe) }));
	CGE::instance->renderer->drawRect(x2, y2 + 27, 530, 48);

	CGE::instance->guiShader->loadInt("texturing", 1);

	if (displayTop) {
		char timebuf[3];
		timebuf[2] = '\0';
		std::tm tm;
		std::time_t t = std::time(nullptr) - timer;
		localtime_s(&tm, &t);
		std::strftime(timebuf, sizeof(timebuf), "%S", &tm);
		std::string st = std::to_string(tm.tm_min) + ":" + timebuf;
		CGE::instance->renderer->drawString(width - (60 + futura->length(st, 15)), y2 + 2, st, Align::LEFT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.98f) * getColor(), 15, futura);
	}
	int ww = light->length(second, 32);
	glm::vec4 sc = glm::mix(Utils::fromHex(0x99c3d3ff), glm::vec4(1, 1, 1, 1), abs(sa * 2 - 1)) * getColor();
	sa += CGE::instance->millis;
	if (sa > 1.f) {
		sa = 0;
	}
	
	CGE::instance->renderer->blur(width - (124 + ww), y2 + 26, ww + 4, 42, [&]() {
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				CGE::instance->renderer->drawString(2 + x, 2 + y, second, Align::LEFT, TextStyle::SIMPLE, Utils::fromHex(0x306396ff), 32, light);
			}
		}
	}, 7);
	if (displayTop)
		CGE::instance->renderer->drawString(width - (120 + futura->length(first, 17)), y2 + 2, first, Align::LEFT, TextStyle::SIMPLE, Utils::fromHex(0x99dfeeff) * getColor(), 17, futura);
	CGE::instance->renderer->drawString(width - (120 + ww), y2 + 30, second, Align::LEFT, TextStyle::SIMPLE, sc, 32, light);
	GuiConnectingPanel::renderWidgets(rf, mouseX + Gui::x, mouseY + Gui::y);
}

void GuiConnectingPanel::onMouseClick(int mouseX, int mouseY, int button) {
	GuiContainer::onMouseClick(mouseX + x, mouseY + y, button);
}
#endif