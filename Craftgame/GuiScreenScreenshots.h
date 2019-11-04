#pragma once
#include <atomic>
#ifndef SERVER
#include "gl.h"
#include "GuiScreen.h"
#include "ImageLoader.h"
#include "AnimationRotate.h"


class GuiScreenScreenshots: public GuiScreen
{
	std::atomic<GLuint> t = 0;
	Image* img = nullptr;
	size_t index = 0;
	std::string imageName;
	void loadImage(std::string url);
	void imagesPage(std::string url);
	std::vector<std::string> urlsTT;
	std::atomic_bool blur = false;
	AnimationRotate anim;
	float flare1 = 0;
	float flare2 = 0;
public:
	GuiScreenScreenshots();
	~GuiScreenScreenshots();
	virtual void render(int flags, int mouseX, int mouseY) override;
	virtual void onLayoutChange(int x, int y, int _width, int _height) override;
	virtual void close() override;
};
#endif