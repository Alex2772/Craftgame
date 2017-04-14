#ifndef SERVER
#include "SoundManager.h"
#include "GameEngine.h"
#include "Parsing.h"
#include <vorbis\vorbisfile.h>

#pragma warning(disable: 4996)

#define ABUFFER_SIZE 32768

ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat ListenerOri[] = { 0.0, 0.0, 0.0,  0.0, 1.0, 0.0 };

SoundManager::SoundManager() {
	INFO("Initializing sound manager");
}

void SoundManager::initializeDevice(const char* name) {
	if (context) {
		alcDestroyContext(context);
	}

	if (device) {
		alcCloseDevice(device);
	}
	device = alcOpenDevice(name);
	if (!device)
		throw std::runtime_error((std::string("Failed to open device ") + name).c_str());
	context = alcCreateContext(device, nullptr);
	alcMakeContextCurrent(context);
	// Устанавливаем параметры слушателя
	// Позиция
	alListenerfv(AL_POSITION, ListenerPos);
	// Скорость
	alListenerfv(AL_VELOCITY, ListenerVel);
	// Ориентация
	alListenerfv(AL_ORIENTATION, ListenerOri);
	checkForError();
}

bool SoundManager::checkForError() {
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR) {
		std::string s = std::string("An OpenAL error has occurred: [") + std::to_string(error) + "] " + alGetString(error);
		CGE::instance->logger->err(s);
		return false;
	}
	return true;
}

SoundManager::~SoundManager() {
	state = false;
	for (size_t i = 0; i < threadPool.size(); i++) {
		threadPool[i]->join();
		delete threadPool[i];
	}
	if (threadPool.size())
		threadPool.clear();
	for (size_t i = 0; i < sounds.size(); i++) {
		delete sounds[i];
	}
	if (sounds.size())
		sounds.clear();
	alcMakeContextCurrent(nullptr);
	if (context) {
		alcDestroyContext(context);
		context = nullptr;
	}

	if (device) {
		alcCloseDevice(device);
		device = nullptr;
	}
}

void SoundManager::registerSound(Sound* s) {
	sounds.push_back(s);
}
void SoundManager::playSoundEffect(SoundEffect* _s) {
	for (size_t i = 0; i < sounds.size(); i++) {
		Sound* s = sounds[i];
		if (s->resource == _s->sound) {
			ALuint source = _s->sourceID;
			ALuint id = s->id;
			threadPool.push_back(new thread([&, source, id, _s]() {
				ALint s;
				alSourcei(source, AL_BUFFER, id);
				alSourcePlay(source);
				checkForError();
				do {
					alGetSourcei(source, AL_SOURCE_STATE, &s);
				} while (s != AL_STOPPED && state);
				delete _s;
			}));
		}
	}
}


//=============================================================================//
//                                  SOUND                                      //
//=============================================================================//

Sound::Sound(_R& _resource, std::string _filename) :
	resource(_resource),
	filename(_filename)
{
	alGenBuffers(1, &id);
	if (Parsing::endsWith(_filename, ".ogg") || Parsing::endsWith(_filename, ".mp3")) {
		std::vector<char> localBuffer;
		int bitStream;
		vorbis_info* vInfo;
		OggVorbis_File file;

		char buffer[ABUFFER_SIZE];

		FILE* f = fopen(_filename.c_str(), "rb");
		ov_open(f, &file, nullptr, 0);
		vInfo = ov_info(&file, -1);
		if (f && vInfo) {
			ALenum format = (channels = vInfo->channels) == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO8;
			freq = vInfo->rate;
			long bytes;
			do {
				bytes = ov_read(&file, buffer, ABUFFER_SIZE, 0, 2, 1, &bitStream);
				localBuffer.insert(localBuffer.end(), buffer, buffer + bytes);
			} while (bytes);
			alBufferData(id, format, &localBuffer[0], localBuffer.size(), freq);
		}
		else {
			WARN(std::string("Failed to load file ") + _filename + ": incorrect ogg format");
		}
		ov_clear(&file);
		if (f)
			fclose(f);

	}
	else {
		std::vector<std::string> spl = Parsing::splitString(_filename, ".");
		throw std::invalid_argument((std::string("Unknown format: ") + spl[spl.size() - 1]).c_str());
	}

	CGE::instance->soundManager->checkForError();
}
Sound::~Sound() {
	alDeleteBuffers(1, &id);
}

//=============================================================================//
//                              SOUND EFFECT                                   //
//=============================================================================//

SoundEffect::SoundEffect(_R& _sound, glm::vec3& _pos, float _volume, float _pitch, bool _rep) :
	sound(_sound),
	pos(_pos),
	volume(_volume),
	repeating(_rep),
	pitch(_pitch) {
	static ALfloat position[] = { _pos.x, _pos.y, _pos.z };
	static ALfloat vel[] = { 0.f, 0.f, 0.f };
	alGenSources(1, &sourceID);
	alSourcef(sourceID, AL_PITCH, pitch);
	alSourcef(sourceID, AL_GAIN, volume);
	alSourcefv(sourceID, AL_POSITION, position);
	alSourcefv(sourceID, AL_VELOCITY, vel);
	alSourcei(sourceID, AL_LOOPING, repeating);
}
SoundEffect::~SoundEffect() {
	alSourceStop(sourceID);
	alDeleteSources(1, &sourceID);
}
#endif