#ifndef SERVER
#include "GuiGraph.h"
#include "GameEngine.h"
#include <iomanip>

GuiGraph::GuiGraph()
{
}

GuiGraph::~GuiGraph()
{
	for (size_t i = 0; i < graphItems.size(); i++) {
		delete graphItems[i];
	}
	graphItems.clear();
}

void GuiGraph::render(int _rf, int mouseX, int mouseY)
{
	Gui::render(_rf, mouseX, mouseY);
	double xvalue = 0;
	for (size_t i = 0; i < graphItems.size(); i++) {
		xvalue += graphItems[i]->value;
	}
	if (xvalue < step * 4)
		xvalue = step * 4;

	int xvalue2 = ceil(xvalue / step) * step;
	if (value < 0) {
		value = xvalue2;
	} else {
		double ds = ((double(xvalue2) - value) / 1.1 + 0.1) * CGE::instance->millis * 7;
		if (xvalue2 < value)
			ds *= 0.08;
		if (value < 0)
			value = xvalue2;
		else if (value != xvalue2) {
			value += ds;
		}
	}

	double mm = value / step;


	CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 0.3));
	CGE::instance->renderer->setTexturingMode(0);
	size_t www = width - 60;
	CGE::instance->renderer->drawRect(30, 10, www, 2);

	for (size_t i = 0; i <= mm * step && i < 200; i++) {
		CGE::instance->renderer->setTexturingMode(0);
		int x = 30 + float(i) / step / (float(mm)) * www;
		if (i % step) {
			if (float(step) / float(mm) > 0.1f) {
				CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 0.5));
				CGE::instance->renderer->drawRect(x - 1, 25, (float(step) / float(mm)) > 0.4f ? 2 : 1, 8);
				if (float(step) / float(mm) >= 1)
					CGE::instance->renderer->drawString(x + 2, 35, std::to_string(i), Align::CENTER, SIMPLE, glm::vec4(1, 1, 1, 0.5), 9);
			}
		}
		else {
			CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 0.9));
			CGE::instance->renderer->drawRect(x - 1, 25, 3, 13);
			CGE::instance->renderer->drawString(x + 2, 39, std::to_string(i), Align::CENTER, SIMPLE, glm::vec4(1, 1, 1, 0.9), 13);
		}
	}
	CGE::instance->renderer->setTexturingMode(0);
	size_t w = 30;
	size_t h = 65;
	mm *= step;
	for (size_t i = 0; i < graphItems.size(); i++) {
		CGE::instance->renderer->setTexturingMode(0);
		GraphItem* d = graphItems[i];
		CGE::instance->renderer->setColor(d->color);
		size_t v = (d->value / double(mm)) * www;
		CGE::instance->renderer->drawRect(w, 10, v, 2);
		w += v;

		CGE::instance->renderer->drawRect(25, h, 20, 20);
		CGE::instance->renderer->drawString(51, h + 3, d->name, Align::LEFT, SIMPLE, glm::vec4(1, 1, 1, 0.4));
		std::stringstream s;
		s << std::setprecision(4) << d->value << " " << d->si;
		CGE::instance->renderer->drawString(width / 2, h + 2, s.str(), Align::LEFT, SIMPLE, glm::vec4(1, 1, 1, 0.4));
		h += 30;
	}
}
#endif