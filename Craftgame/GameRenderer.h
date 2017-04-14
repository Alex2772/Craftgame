#pragma once
#include "Model.h"
#include "AdvancedModel.h"
#include "Font.h"
#include <functional>

enum Align {
	LEFT,
	CENTER,
	RIGHT
};
enum TextStyle {
	SIMPLE,
	SHADOW
};

class GameRenderer {
public:
	GameRenderer();
	void renderModel(SimpleModel* model);
	void renderModel(AdvancedModel* model, bool materialBinding = true);
	void drawRect(int x, int y, int width, int height, float u, float v, float u2, float v2);
	void drawRect(int x, int y, int width, int height);
	void drawRect(int x, int y, int width, int height, float u, float v);
	void blur(int x, int y, int width, int height, int radius = 13);
	void blur(int x, int y, int width, int height, std::function<void()> func, int radius = 13);
	void drawString(int x, int y, std::string text, Align align = Align::LEFT, TextStyle style = TextStyle::SIMPLE, glm::vec4 color = glm::vec4(1, 1, 1, 1), long size = FONT_SIZE, Font* font = nullptr);
	glm::mat4 transform = glm::mat4(1.0);
	glm::vec4 color = glm::vec4(1.0);
	int viewportX;
	int viewportY;
	int viewportW;
	int viewportH;
	void changeViewport(int x, int y, int width, int height);
};