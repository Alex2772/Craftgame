#include "PWorldTime.h"
#include "GameEngine.h"

void PWorldTime::write(ByteBuffer& buffer)
{
	buffer << time;
}

void PWorldTime::read(ByteBuffer& buffer)
{
	buffer >> time;
}

void PWorldTime::onReceived()
{
#ifndef SERVER
	CGE::instance->thePlayer->worldObj->setTime(time);
#endif
}

void PWorldTime::onReceived(EntityPlayerMP*)
{
}
