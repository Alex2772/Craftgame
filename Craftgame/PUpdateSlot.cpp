#include "PUpdateSlot.h"

#ifndef SERVER
#include "GameEngine.h"
extern float* popAnim;
#endif

void PUpdateSlot::write(ByteBuffer& buffer)
{
	buffer << stack;
	buffer << slot;
}

void PUpdateSlot::read(ByteBuffer& buffer)
{
	buffer >> stack;
	buffer >> slot;
}

void PUpdateSlot::onReceived()
{
#ifndef SERVER
	if (slot == 0)
	{
		if (CGE::instance->thePlayer->tmpInv)
			delete CGE::instance->thePlayer->tmpInv;
		CGE::instance->thePlayer->tmpInv = stack;
		return;
	} else
	{
		--slot;
	}
	size_t s;
	InventoryContainer* container;
	if (CGE::instance->thePlayer->interface->slotByIndex(CGE::instance->thePlayer->inventory, slot, container, s))
	{
		Slot*& slot = container->slots[s];
		if (popAnim[s] > 0.5f) {
			if (!(slot->stack && stack) || slot->stack->getCount() != stack->getCount())
				popAnim[s] = 1.f - popAnim[s];
		}
		kek:
		if (slot->stack)
		{
			delete slot->stack;
		}
		slot->stack = stack;
	}
#endif
}

void PUpdateSlot::onReceived(EntityPlayerMP*)
{
}
