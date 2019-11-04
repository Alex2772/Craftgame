#pragma once

#include "Animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "global.h"
#include <string>

class AnimationRotate : public Animation {
public:
	AnimationRotate(Gui* _g) :
		Animation(_g, 0) {

	}
	virtual void process() {
		Animation::process();

		gui->animTransform = glm::rotate(glm::mat4(1.0f), glm::radians(sin(frame * 0.02f)) * 4, glm::vec3(0, 1, 0));
		gui->animTransform = glm::rotate(gui->animTransform, glm::radians(cos(frame * 0.02f)) * 4, glm::vec3(1, 0, 0));
		if (frame * 0.01f > M_PI)
			frame = 0;
	}
};