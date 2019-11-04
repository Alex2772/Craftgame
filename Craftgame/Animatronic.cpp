#include "Animatronic.h"

SkeletalAnimation& Animatronic::applyAnimation(std::string name, char initial, bool repeating)
{
	mtype::iterator fnd = mAnimations.find(name);
	if (fnd == mAnimations.end()) {
		SkeletalAnimation s;
		s.state = initial;
		if (repeating)
			s.state |= 0x80;
		mAnimations[name] = s;
		return mAnimations[name];
	}
	else {
		SkeletalAnimation& s = fnd->second;
		s.state = initial;
		if (repeating)
			s.state |= 0x80;
		s.time = 0.f;
		s.value = 0.f;
		return s;
	}
}
SkeletalAnimation& Animatronic::updateAnimation(std::string name, char initial, bool repeating)
{
	mtype::iterator fnd = mAnimations.find(name);
	if (fnd == mAnimations.end()) {
		SkeletalAnimation s;
		s.state = initial;
		if (repeating)
			s.state |= 0x80;
		mAnimations[name] = s;
		return mAnimations[name];
	}
	else {
		SkeletalAnimation& s = fnd->second;
		s.state = initial;
		if (repeating)
			s.state |= 0x80;
		if (s.value == 0.f) {
			s.time = 0.f;
		}
		return s;
	}
}