#pragma once
#include "GuiProgressBar.h"

#ifndef SERVER
#include "GuiWindow.h"
#include "GuiList.h"
#include "GuiLabel.h"
#include "GuiButton.h"
#include "GuiGraph.h"
#include "Utils.h"
#include "GameEngine.h"
#include "PseudoRandom.h"

void __drw(int x, int y, std::string t) {
	CGE::instance->renderer->drawString(x, y, t, Align::LEFT, SIMPLE, glm::vec4(1, 1, 1, 0.6), 11);
}

class GuiWindowDebug_UI : public Gui {
public:
	GuiWindowDebug_UI() {}
	virtual void render(int _rf, int mouseX, int mouseY) {
		Gui::render(_rf, mouseX, mouseY);
		__drw(10, 10, "FPS " + std::to_string((int)round(1.f / CGE::instance->millis)));
		__drw(10, 22, "millis " + std::to_string((int)round(CGE::instance->millis * 1000)));
	}
};
class GuiWindowDebug_UI_Random : public Gui {
public:
	PseudoRandom rnd;
	double chance;
	size_t proc = 0;
	size_t first = 0;
	time_t lastupdate = 0;
	float highlight = 0;
	GuiWindowDebug_UI_Random(double c):
		rnd(c),
		chance(c)
	{
		height = 16;
	}
	void update() {
		proc = 0;
		first = 0;
		for (size_t i = 0; i < 100; i++) {
			if (rnd.next()) {
				if (!first)
					first = i + 1;
				proc++;
			}
		}
	}
	virtual void render(int _rf, int mouseX, int mouseY) {
		if (lastupdate < time(0)) {
			lastupdate = time(0);
			update();
		}
		Gui::render(_rf, mouseX, mouseY);

		if (_rf & RenderFlags::MOUSE_HOVER)
			highlight += CGE::instance->millis * 3.5f;
		else
			highlight -= CGE::instance->millis * 2.5f;
		highlight = glm::clamp(highlight, 0.f, 1.f);
		CGE::instance->renderer->setTexturingMode(0);
		CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 0.2f * highlight));
		CGE::instance->renderer->drawRect(0, 0, width, height);

		__drw(10, 2, std::to_string(int(chance)) + "%");
		CGE::instance->renderer->drawString(150, 2, std::to_string(rnd.cr), Align::CENTER, SIMPLE, glm::vec4(1, 1, 1, 0.5), 11);
		CGE::instance->renderer->drawString(100, 2, std::to_string(proc) + "%", Align::RIGHT, SIMPLE, glm::vec4(1, 1, 1, 0.5), 11);
	}
};
class GuiWindowDebug : public GuiWindow {
private:
	GuiGraph* s;
	GuiGraph* s2;
	GuiProgressBar* s3;
public:
	GuiWindowDebug():
		GuiWindow("Debug", 400, 300)
	{
		classname = "debug";
		GuiList* l = new GuiList;

		{ 
			{
				GuiLabel * k = new GuiLabel("UI");
				k->onLayoutChange(0, 0, 400, 60);
				k->color = glm::vec4(1, 1, 1, 0.7);
				k->setTextSize(30);
				l->addWidget(k);
			}
			s = new GuiGraph;
			
			s->graphItems.push_back(new GraphItem{ Utils::fromHex(0x6bc8e6ff), "UI THREAD", "ms", 0 });
			s->graphItems.push_back(new GraphItem{ Utils::fromHex(0x33a743ff), "PRERENDER", "ms", 0 });
			s->graphItems.push_back(new GraphItem{ Utils::fromHex(0x378deeff), "GUI", "ms", 0 });
			s->graphItems.push_back(new GraphItem{ Utils::fromHex(0x26ee42ff), "RENDER", "ms", 0 });
			s->height = 180;
			l->addWidget(s);
			{
				GuiLabel * k = new GuiLabel("GUI");
				k->onLayoutChange(0, 0, 400, 60);
				k->color = glm::vec4(1, 1, 1, 0.7);
				k->setTextSize(30);
				l->addWidget(k);
			}
			s2 = new GuiGraph;

			s2->graphItems.push_back(new GraphItem{ Utils::fromHex(0x00ffc0ff), "RECT", "ms", 0 });
			s2->graphItems.push_back(new GraphItem{ Utils::fromHex(0xea00ffff), "BLUR", "ms", 0 });
			s2->graphItems.push_back(new GraphItem{ Utils::fromHex(0xffde00ff), "TEXT", "ms", 0 });
			s2->height = 140;
			l->addWidget(s2);
			s3 = new GuiProgressBar;
			{
				GuiLabel * k = new GuiLabel("Memory usage");
				k->onLayoutChange(0, 0, 400, 60);
				k->color = glm::vec4(1, 1, 1, 0.7);
				k->setTextSize(30);
				l->addWidget(k);
			}
			l->addWidget(s3);
			l->addWidget(new GuiWindowDebug_UI);
			for (size_t i = 0; i < 20; i++) {
				l->addWidget(new GuiWindowDebug_UI_Random((i + 1) * 5));
			}
		}
		addWidget(l);
	}
	virtual void render(int _rf, int mouseX, int mouseY) {
		_rf |= NO_BLUR;
		GuiWindow::render(_rf, mouseX, mouseY);
		s->graphItems[0]->value = double(CGE::instance->debugger.getSection("uithread")->prev.count()) / 1000;
		s->graphItems[1]->value = double(CGE::instance->debugger.getSection("prerender")->prev.count()) / 1000;
		s->graphItems[2]->value = double(CGE::instance->debugger.getSection("ui")->prev.count()) / 1000;
		s->graphItems[3]->value = double(CGE::instance->debugger.getSection("render")->prev.count()) / 1000;

		s2->graphItems[0]->value = double(CGE::instance->debugger.getTTimer("rect")->prev.count()) / 1000;
		s2->graphItems[1]->value = double(CGE::instance->debugger.getTTimer("blur")->prev.count()) / 1000;
		s2->graphItems[2]->value = double(CGE::instance->debugger.getTTimer("text")->prev.count()) / 1000;


		GLint total_mem_kb = 0;
		glGetIntegerv(0x9048,
			&total_mem_kb);

		GLint cur_avail_mem_kb = 0;
		glGetIntegerv(0x9049,
			&cur_avail_mem_kb);
		s3->si = " MB";
		s3->max = float((total_mem_kb) / 1024);
		s3->value = float((total_mem_kb - cur_avail_mem_kb) / 1024);
	}
	virtual void onLayoutChange(int x, int y, int width, int height) {
		widgets[0]->onLayoutChange(0, 0, width, height - 35);
	}
};
#endif