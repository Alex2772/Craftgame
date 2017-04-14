#ifndef SERVER
#include "Animation.h"
#include "GameEngine.h"

#define ANIMATION_SPEED 1

Animation::Animation(Gui* _g, float _maxFrames):
gui(_g),
maxFrames(_maxFrames) {

}
void Animation::process() {
	if (state == State::play)
		frame += CGE::instance->millis * 10 * ANIMATION_SPEED;
	else if (state == State::inverse)
		frame -= CGE::instance->millis * 10 * ANIMATION_SPEED;
	if (frame < 0) {
		frame = 0;
		if (onAnimationEnd)
			onAnimationEnd(state);
		state = State::stop;
	}
	else if (frame > maxFrames) {
		frame = maxFrames;
		if (onAnimationEnd)
			onAnimationEnd(state);
		state = State::stop;
	}
}
void Animation::setAnimationState(State s) {
	state = s;
	if (s == State::play)
		frame = 0;
	else if (s == State::inverse)
		frame = maxFrames;
}
#endif