#include "PCharging.h"
#include "ItemCharging.h"

#include "GameEngine.h"

PCharging::PCharging(EntityPlayerMP* sabj, uint8_t s):
	PCharging()
{
	id = sabj->getId();
	state = s;
}

void PCharging::write(ByteBuffer& buffer)
{
	buffer << id << state;
}

void PCharging::read(ByteBuffer& buffer)
{
	buffer >> id >> state;
}

void PCharging::onReceived()
{
#ifndef SERVER
	if (EntityPlayerMP* pl = dynamic_cast<EntityPlayerMP*>(CGE::instance->thePlayer->worldObj->getEntity(id)))
	{
		if (pl->getHeldItem())
		{
			if (IStackCharging* s = dynamic_cast<IStackCharging*>(pl->getHeldItem()))
			{
				switch (state)
				{
				case 0:
					if (s->isUsed)
					{
						dynamic_cast<ItemCharging*>(pl->getHeldItem()->getItem())->stopCharging(pl->getHeldItem(), pl);
					}
					break;
				case 1:
					if (!s->isUsed)
					{
						dynamic_cast<ItemCharging*>(pl->getHeldItem()->getItem())->startCharging(pl->getHeldItem(), pl);
					}
					break;
				}
			}
		}
	}
#endif
}

void PCharging::onReceived(EntityPlayerMP* pl)
{
	if (pl->getHeldItem())
	{
		if (IStackCharging* s = dynamic_cast<IStackCharging*>(pl->getHeldItem()))
		{
			switch (state)
			{
			case 0:
				if (s->isUsed)
				{
					dynamic_cast<ItemCharging*>(pl->getHeldItem()->getItem())->stopCharging(pl->getHeldItem(), pl);
				}
				break;
			case 1:
				if (!s->isUsed)
				{
					dynamic_cast<ItemCharging*>(pl->getHeldItem()->getItem())->startCharging(pl->getHeldItem(), pl);
				}
				break;
			}
			if (!pl->worldObj->isRemote)
			{
				ts<IServer>::r(CGE::instance->server)->sendPacketAllExcept(new PCharging(pl, state), pl->getNetHandler());
			}
		}
	}
}
