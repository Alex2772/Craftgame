#pragma once

#include "Animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "global.h"
#include <string>

class AnimationOpen : public Animation {
public:
	AnimationOpen(Gui* _g) :
		Animation(_g, 10) {

	}
	virtual void process() {
		Animation::process();
		glm::mat4 tr = glm::mat4(1.0);
		glm::vec4 color(1);
		tr = glm::translate(tr, glm::vec3(-(cos(frame / M_PI) + 1) / 7.f, (cos(frame / M_PI) + 1) / 7.f, 0));
		color.a = (cos((frame + 10) / M_PI) + 1) / 2;
		gui->animColor *= color;
		gui->animTransform *= tr;
	}
};