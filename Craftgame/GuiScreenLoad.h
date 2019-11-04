#pragma once
#include "Framebuffer.h"
#ifndef SERVER
#include "GuiScreen.h"
#include "Block.h"
#include "LightSource.h"
#include "Animation.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "SecFramebuffer.h"

class AnimationGSL : public Animation {
public:
	AnimationGSL(Gui* _g) :
		Animation(_g, 8) {

	}
	
	virtual void process() {
		Animation::process();
		glm::vec4 color(1);
		float d = 1.f - frame / maxFrames;
		d = pow(d, 2);
		color.a = 1.f - d;
		glm::mat4 m = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 1.f + d * 0.01f, d * 0.2f));
		m = glm::rotate(m, -glm::radians(d * 4.f), glm::vec3(1, 0, 0));
		m = glm::translate(m, glm::vec3(0, -1, 0));
		m = glm::scale(m, glm::vec3(1, 1, 1) * 0.7f * d + 1.f);
		gui->animTransform *= m;
		gui->animColor *= color;
	}
};

class GuiScreenLoad : public GuiScreen {
private:
	Block* b;
	std::string displayString;
public:
	LightSource* l;
	LightSource* l2;
	Framebuffer* fb = nullptr;
	virtual void onLayoutChange(int _x, int _y, int _width, int _height);
	GuiScreenLoad();
	~GuiScreenLoad();
	virtual void animatedClose(Animation* a);
	float a = 0.f;
	virtual void render(int _rf, int mouseX, int mouseY);
	void setDisplayStringAsync(std::string s);
};
#endif