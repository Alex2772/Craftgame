#include "InventoryPlayerContainer.h"
#include "EntityPlayerMP.h"
#include "PUpdateSlot.h"
#include "IServer.h"
#include "GameEngine.h"
#include "PItemHeld.h"

void InventoryPlayerContainer::slotUpdated(size_t index)
{
	if (!player->worldObj->isRemote) {
		PUpdateSlot* p = new PUpdateSlot;
		p->slot = index + 1;
		p->stack = slots[index]->stack;
		player->getNetHandler()->sendPacket(p);
		if (player->getItemIndex() == index)
		{
			ts<IServer>::r(CGE::instance->server)->sendPacketAllExcept(new PItemHeld(player, player->getHeldItem()), player->getNetHandler());
		}
	}
}

InventoryPlayerContainer::InventoryPlayerContainer(EntityPlayerMP* p):
	player(p)
{
}

ItemStack* InventoryPlayerContainer::swap(size_t index, ItemStack* tmp)
{
	ItemStack* s = InventoryContainer::swap(index, tmp);
	slotUpdated(index);
	return s;
}

bool InventoryPlayerContainer::get(size_t index, uint16_t count, ItemStack*& dst)
{
	if (InventoryContainer::get(index, count, dst))
	{
		slotUpdated(index);
		return true;
	}
	return false;
}

bool InventoryPlayerContainer::put(size_t index, uint16_t count, ItemStack*& from)
{
	if (InventoryContainer::put(index, count, from))
	{
		slotUpdated(index);
		return true;
	}
	return false;
}
