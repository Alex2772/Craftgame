#include "PContainerAction.h"
#include "PContainerData.h"
#include "GameEngine.h"
#include "PUpdateSlot.h"

void PContainerAction::write(ByteBuffer& buffer)
{
	buffer << count << from << to;
}

void PContainerAction::read(ByteBuffer& buffer)
{
	buffer >> count >> from >> to;
}

void PContainerAction::onReceived()
{
}
bool PContainerAction::process(EntityPlayerMP*& p)
{
	if (from == to)
		return false;
	InventoryContainer* fromContainer = nullptr;
	size_t slot = 0;
	ItemStack** fromStack = nullptr;
	if (from == 0) {
		fromStack = &p->tmpInv;
	} else
	{
		if (p->interface->slotByIndex(p->inventory, from - 1, fromContainer, slot))
		{
			fromStack = &fromContainer->slots[slot]->stack;
		}
	}
	if (fromStack == nullptr)
	{
		return false;
	}

	if (to == 0) {
		if (!fromContainer)
			return false;
		return fromContainer->get(slot, count, p->tmpInv);
	} else {
		InventoryContainer* inv;
		if (p->interface->slotByIndex(p->inventory, to - 1, inv, slot))
		{
			return inv->put(slot, count, *fromStack);
		}
	}
	return false;
}
void PContainerAction::onReceived(EntityPlayerMP* p)
{
	if (p->interface) {
		if (process(p)) {
			for (auto& c : p->interface->players)
			{
				if (c.first != p)
				{
					PUpdateSlot* o = new PUpdateSlot();
					if (to != 0)
					{
						o->slot = to;
					} else
					{
						o->slot = from;
					}
					if (o->slot) {
						InventoryContainer* inv;
						size_t id;
						if (p->interface->slotByIndex(p->inventory, o->slot - 1, inv, id))
						{
							if (inv != p->inventory) {
								o->stack = inv->slots[id]->stack;
								c.first->getNetHandler()->sendPacket(o);
								return;
							}
						}
					}
					delete o;
				}
			}
		} else 
		{
			p->getNetHandler()->sendPacket(new PContainerData(p, p->interface));
		}
	}
}
