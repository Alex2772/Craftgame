#include "PContainerData.h"
#include "GameEngine.h"
#include "GuiInventory.h"

void PContainerData::write(ByteBuffer& buffer)
{
	ts<IServer>::r lock(CGE::instance->server);
	buffer << player->tmpInv;
	buffer << player->inventory;
	if (cs)
		for (InventoryContainer*& c : cs->containers)
			buffer << c;
}

void PContainerData::read(ByteBuffer& b)
{
	buffer = b;
}

void PContainerData::onReceived()
{
#ifndef SERVER
	if (CGE::instance->thePlayer->tmpInv) {
		delete CGE::instance->thePlayer->tmpInv;
		CGE::instance->thePlayer->tmpInv = nullptr;
	}
	buffer >> CGE::instance->thePlayer->tmpInv;
	if (CGE::instance->thePlayer->interface)
	{
		buffer >> CGE::instance->thePlayer->inventory;
		for (InventoryContainer*& c : CGE::instance->thePlayer->interface->containers)
			buffer >> c;
	} else
	{
		buffer >> CGE::instance->thePlayer->inventory;
	}
#endif
}

void PContainerData::onReceived(EntityPlayerMP* player)
{
}
