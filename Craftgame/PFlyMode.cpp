#include "PFlyMode.h"

PFlyMode::PFlyMode(bool b):
	PFlyMode()
{
	flyMode = b;
}

void PFlyMode::write(ByteBuffer & buffer)
{
	buffer << flyMode;
}

void PFlyMode::read(ByteBuffer& buffer)
{
	buffer >> flyMode;
}
void PFlyMode::onReceived()
{
	
}
void PFlyMode::onReceived(EntityPlayerMP* p)
{
	p->setFlying(flyMode);
}