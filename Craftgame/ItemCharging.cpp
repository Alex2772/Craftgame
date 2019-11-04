#include "ItemCharging.h"
#include "GameEngine.h"
#include "PCharging.h"

IStackCharging::~IStackCharging()
{
	EntityPlayerMP* player = nullptr;

	ts<IServer>::r l(CGE::instance->server);
	if (CGE::instance->server.isValid()) {
		if (l->getPlayers()) {
			for (NetPlayerHandler*& k : *l->getPlayers())
			{
				if (static_cast<void*>(k->getPlayer()->getHeldItem()) == static_cast<void*>(this))
				{
					k->getPlayer()->itemCharging = nullptr;
					player = k->getPlayer();
					break;
				}
			}
		}
		if (isUsed)
		{
			if (ItemStack* kek = dynamic_cast<ItemStack*>(this)) {
				if (ItemCharging* ax = dynamic_cast<ItemCharging*>(kek->getItem()))
					ax->stopCharging(dynamic_cast<ItemStack*>(this), player);
			}
		}
	}
}

void ItemCharging::tick(ItemStack* stack, EntityPlayerMP* sabj)
{
	if (IStackCharging* s = dynamic_cast<IStackCharging*>(stack))
	{
		if (s->isUsed)
			s->ticksCharging++;
	}
}

void ItemCharging::startCharging(ItemStack* s, EntityPlayerMP* mp)
{
	mp->itemCharging = s;
	IStackCharging* k = dynamic_cast<IStackCharging*>(s);
	k->isUsed = true;
	k->ticksCharging = 0;
#ifndef SERVER
	if (mp->worldObj->isRemote && CGE::instance->thePlayer == mp)
	{
		ts<IServer>::r(CGE::instance->server)->sendPacket(new PCharging(mp, 1));
	}
#endif
}

void ItemCharging::stopCharging(ItemStack* s, EntityPlayerMP* mp)
{
	IStackCharging* k = dynamic_cast<IStackCharging*>(s);
	k->isUsed = false;
	ts<IServer>::r lock(CGE::instance->server);
#ifndef SERVER
	if (mp->worldObj->isRemote && CGE::instance->thePlayer == mp)
	{
		lock->sendPacket(new PCharging(mp, 0));
	}
#endif
	mp->itemCharging = nullptr;
}
