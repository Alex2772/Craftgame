#pragma once

#include "Animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "global.h"
#include <string>

class AnimationDialog : public Animation {
public:
	AnimationDialog(Gui* _g) :
		Animation(_g, 4) {

	}
	virtual void process() {
		Animation::process();
		glm::vec4 color(1);
		color.a = 1 - (cos(frame / M_PI * 2.25f) + 1) / 2;
		float d = frame / maxFrames;
		glm::mat4 m = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 1.f + (1 - frame / maxFrames) * 0.01f, (1 - frame / maxFrames) * 0.2f));
		m = glm::rotate(m, -glm::radians((1 - d * d * d) * 8.f), glm::vec3(1, 0, 0));
		m = glm::translate(m, glm::vec3(0, -1, 0));
		gui->animTransform *= m;
		gui->animColor *= color;
	}
};
