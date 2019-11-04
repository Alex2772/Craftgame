#pragma once
#ifndef SERVER
#include "Gui.h"
#include <vector>

struct GraphItem {
	glm::vec4 color;
	std::string name;
	std::string si;
	double value;
};

class GuiGraph : public Gui {
public:
	double value = -1;
	GuiGraph();
	~GuiGraph();
	virtual void render(int _rf, int mouseX, int mouseY);
	std::vector<GraphItem*> graphItems;
	size_t step = 5;
};
#endif