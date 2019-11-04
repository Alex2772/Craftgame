#pragma once
#ifndef SERVER
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <thread>
#include "Res.h"
#include <glm/glm.hpp>
#include "global.h"
#include <deque>

class Sound {
public:
	long freq = 0;
	byte channels = 0;
	ALuint id = 0;
	_R resource;
	std::string filename;
	Sound(_R& resource, std::string filename);
	~Sound();
};

class SoundEffect {
public:
	_R sound;
	glm::vec3 pos;
	float volume;
	float pitch;
	bool repeating;
	SoundEffect(_R& sound, glm::vec3& pos, float volume = 1.f, float pitch = 1.f, bool repeating = false);
	~SoundEffect();
	ALuint sourceID;
};

class SoundManager {
private:
	ALCdevice* device = nullptr;
	ALCcontext* context = nullptr;
	std::vector<Sound*> sounds;
	std::deque<SoundEffect*> se;
	bool state = true;
public:
	ALuint uiEffectSlot[4] = { 0 };
	ALuint uiEffect[2] = { 0 };
	ALuint uiFilter[1] = { 0 };
	SoundManager();
	void setListener(glm::vec3 pos, glm::vec3 o);

	bool checkForError();
	void registerSound(Sound* s);
	void playSoundEffect(SoundEffect* s);
	void initializeDevice(const char* name);
	
	~SoundManager();
};
#endif