#include "CSInterface.h"
#include "GameEngine.h"
#include "POpenInventory.h"
#include "EntityItem.h"
#ifndef SERVER
#include "GuiInventory.h"
#endif

CSInterface::CSInterface(CSInterface** p, std::vector<InventoryContainer*> c):
containers(c),
ptr(p)
{
}

CSInterface::~CSInterface()
{
	CGE::instance->threadPool.runAsync([] (std::map<EntityPlayerMP*, InventoryContainer*> players)
	{
		ts<IServer>::rw lock(CGE::instance->server);
		for (auto& p : players)
		{
			p.first->interface = nullptr;
		}
	}, players);
	*ptr = nullptr;
}

size_t CSInterface::indexOfSlot(size_t container, size_t index) const
{
	index += 36;
	for (size_t i = 0; i <= container; i++)
	{
		index += containers[i]->slots.size();
	}

	return index;
}
void CSInterface::removePlayer(EntityPlayerMP* index)
{
	if (!players.empty())
	{
		std::map<EntityPlayerMP*, InventoryContainer*>::iterator i = players.find(index);
		index->interface = nullptr;
		if (i != players.end()) {
			players.erase(i);
		}
		if (!index->worldObj->isRemote) {
			if (index->tmpInv)
			{
				EntityItem* s = new EntityItem(nullptr, CGE::instance->assignGlobalUniqueEntityId(), index->tmpInv);
				index->worldObj->spawnEntity(s, index->getEyePos());
				s->motion = index->getLookVec() * 3.f;
				index->tmpInv = nullptr;
			}
		}
	}
	if (players.empty())
	{
		delete this;
	}
}
bool CSInterface::slotByIndex(InventoryContainer*& player, size_t index, InventoryContainer*& inv,  size_t& id)
{
	if (index < player->slots.size())
	{
		inv = player;
		id = index;
		return true;
	}
	index -= player->slots.size();
	for (InventoryContainer*& c : containers)
	{
		if (index < c->slots.size())
		{
			inv = c;
			id = index;
			return true;
		} else
		{
			index -= c->slots.size();
		}
	}
	return false;
}
