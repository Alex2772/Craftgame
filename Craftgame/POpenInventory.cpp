#include "POpenInventory.h"
#include "PContainerData.h"
#include "GameEngine.h"
#include "GuiInventory.h"

void POpenInventory::write(ByteBuffer& buffer)
{
}

void POpenInventory::read(ByteBuffer& buffer)
{
}

void POpenInventory::onReceived()
{
}

void POpenInventory::onReceived(EntityPlayerMP* p)
{
	if (p->interface) {
		p->interface->removePlayer(p);
	} else {
		p->openInterface(new CSInterface(&p->interface, {}));
	}
}
