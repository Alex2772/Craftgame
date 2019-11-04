#include "PSound.h"
#include "GameEngine.h"
#include "Random.h"

void PSound::write(ByteBuffer& buffer)
{
	buffer << name;
	buffer << pos;
}

void PSound::read(ByteBuffer& buffer)
{
	buffer >> name;
	buffer >> pos;
}

void PSound::onReceived()
{
#ifndef SERVER
	CGE::instance->soundManager->playSoundEffect(new SoundEffect(_R(name), pos.toVec3(), 1.f, Random::nextFloat() * 0.2f + 0.9f));
#endif
}

void PSound::onReceived(EntityPlayerMP*)
{
}
