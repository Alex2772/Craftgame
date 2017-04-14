#pragma once

#include "Animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "global.h"
#include <string>

class AnimationFade : public Animation {
public:
	AnimationFade(Gui* _g) :
		Animation(_g, 2) {

	}
	virtual void process() {
		Animation::process();
		glm::vec4 color(1);
		color.a = (cos((frame + 2) / M_PI * 5) + 1) / 4 + 0.5;
		gui->animColor *= color;
	}
};