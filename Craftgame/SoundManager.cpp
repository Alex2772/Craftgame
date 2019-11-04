#ifndef SERVER
#include "SoundManager.h"
#include "GameEngine.h"
#include "Parsing.h"
#include <vorbis/vorbisfile.h>
#include <al/efx.h>

#pragma warning(disable: 4996)

#define ABUFFER_SIZE 32768

ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat ListenerOri[] = { 0.0, 0.0, 0.0,  0.0, 1.0, 0.0 };

SoundManager::SoundManager() {
	INFO("Initializing sound manager");
}

void SoundManager::setListener(glm::vec3 pos, glm::vec3 o)
{
	ListenerOri[0] = o.x;
	ListenerOri[1] = o.y;
	ListenerOri[2] = o.z;
	// Позиция
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	// Ориентация
	alListenerfv(AL_ORIENTATION, ListenerOri); 

	for (std::deque<SoundEffect*>::iterator i = se.begin(); i != se.end();)
	{
		SoundEffect* s = *i;
		ALenum state;
		alGetSourcei(s->sourceID, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING)
		{
			delete s;
			i = se.erase(i);
		} else
		{
			++i;
		}
	}
}

LPALGENEFFECTS alGenEffects;
LPALDELETEEFFECTS alDeleteEffects;
LPALISEFFECT alIsEffect;
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
LPALEFFECTI alEffecti;
LPALEFFECTF alEffectf;
LPALGENFILTERS alGenFilters;
LPALISFILTER alIsFilter;
LPALFILTERF alFilterf;
LPALFILTERI alFilteri;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;

void SoundManager::initializeDevice(const char* name) {
	if (context) {
		alcDestroyContext(context);
	}

	if (device) {
		alcCloseDevice(device);
	}
	device = alcOpenDevice(name);
	if (device) {
		if (alcIsExtensionPresent(device, "ALC_EXT_EFX") == AL_FALSE)
			throw std::runtime_error("ALC_EXT_EFX extension is not present");

		ALint attribs[4] = { 0 };
		attribs[0] = ALC_MAX_AUXILIARY_SENDS;
		attribs[1] = 4;

		context = alcCreateContext(device, attribs);
		alcMakeContextCurrent(context);
		alGenEffects = (LPALGENEFFECTS)
			alGetProcAddress("alGenEffects");
		alDeleteEffects = (LPALDELETEEFFECTS)
			alGetProcAddress("alDeleteEffects");
		alIsEffect = (LPALISEFFECT)
			alGetProcAddress("alIsEffect");
		alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
		alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
		alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
		alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
		alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
		alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
		alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
		alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
		/* Check function pointers are valid */
		if (!(alGenEffects && alDeleteEffects && alIsEffect && alGenAuxiliaryEffectSlots))
			throw std::runtime_error("AL ext functions are not present");
		// Устанавливаем параметры слушателя
		// Позиция
		alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
		// Скорость
		alListenerfv(AL_VELOCITY, ListenerVel);
		// Ориентация
		alListenerfv(AL_ORIENTATION, ListenerOri);
		alListenerf(AL_METERS_PER_UNIT, 1.f);
		checkForError();

		for (int i = 0; i < 4; i++)
		{
			alGenAuxiliaryEffectSlots(1, &uiEffectSlot[i]);
			if (alGetError() != AL_NO_ERROR)
				break;
		}

		/* Try to create 2 Effects */
		for (int i = 0; i < 2; i++)
		{
			alGenEffects(1, &uiEffect[i]);
			if (alGetError() != AL_NO_ERROR)
				break;
		}
		checkForError();
		if (alIsEffect(uiEffect[0]))
		{
			alEffecti(uiEffect[0], AL_EFFECT_TYPE, AL_EFFECT_REVERB);
			if (alGetError() != AL_NO_ERROR)
				printf("Reverb Effect not supported\n");
			else
				alEffectf(uiEffect[0], AL_REVERB_DECAY_TIME, 5.0f);
		}
		/* Set second Effect Type to Flanger and change Phase */
		checkForError();
		if (alIsEffect(uiEffect[1]))
		{
			alEffecti(uiEffect[1], AL_EFFECT_TYPE, AL_EFFECT_FLANGER);
			if (alGetError() != AL_NO_ERROR)
				printf("Flanger effect not support\n");
			else
				alEffecti(uiEffect[1], AL_FLANGER_PHASE, 180);
		}
		/* Try to create a Filter */
		checkForError();
		alGenFilters(1, &uiFilter[0]);
		if (alGetError() == AL_NO_ERROR)
			printf("Generated a Filter\n");
		if (alIsFilter(uiFilter[0]))
		{
			/* Set Filter type to Low-Pass and set parameters */
			alFilteri(uiFilter[0], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			if (alGetError() != AL_NO_ERROR)
				printf("Low Pass Filter not supported\n");
			else
			{
				alFilterf(uiFilter[0], AL_LOWPASS_GAIN, .1f);
				alFilterf(uiFilter[0], AL_LOWPASS_GAINHF, .1f);
			}
		}
		alAuxiliaryEffectSloti(uiEffectSlot[0],
			AL_EFFECTSLOT_EFFECT, uiEffect[0]);
		checkForError();
	} else {
		WARN(std::string("Sound device ") + name + " has not initialized");
	}
}

bool SoundManager::checkForError() {
	if (device) {
		ALenum error;
		if ((error = alGetError()) != AL_NO_ERROR) {
			std::string s = std::string("An OpenAL error has occurred: [") + std::to_string(error) + "] " + alGetString(error);
			CGE::instance->logger->err(s);
			return false;
		}
	}
	return true;
}

SoundManager::~SoundManager() {
	state = false;
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
	if (device) {
		for (size_t i = 0; i < sounds.size(); i++) {
			Sound* s = sounds[i];
			if (s->resource == _s->sound) {
				ALuint source = _s->sourceID;
				ALuint id = s->id;
				alSourcei(source, AL_BUFFER, id);
				alSourcePlay(source);
				checkForError(); // TODO delete SoundEffect*
				se.push_back(_s);
			}
		}
	} else
	{
		delete _s;
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
			ALenum format = (channels = vInfo->channels) == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
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
	alSourcef(sourceID, AL_MAX_DISTANCE, 15.f);
	alSource3i(sourceID, AL_AUXILIARY_SEND_FILTER, CGE::instance->soundManager->uiEffectSlot[0],
		0, CGE::instance->soundManager->uiFilter[0]);
	alSource3i(sourceID, AL_AUXILIARY_SEND_FILTER,
		CGE::instance->soundManager->uiEffectSlot[0], 0, AL_FILTER_NULL);
	alSourcef(sourceID, AL_CONE_OUTER_GAINHF, 0.5f);
}
SoundEffect::~SoundEffect() {
	alSourceStop(sourceID);
	alDeleteSources(1, &sourceID);
}
#endif
