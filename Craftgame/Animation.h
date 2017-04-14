#pragma once

#include "Gui.h"

enum State {
	play,
	stop,
	inverse
};

class Animation {
protected:
	Gui* gui;
	float maxFrames;
public:
	Animation(Gui* _g, float maxFrames);
	float frame = 0;
	State state = State::play;
	virtual void process();
	void setAnimationState(State s);
	std::function<void(State)> onAnimationEnd;
};