#pragma once
#include <map>
#include "InventoryContainer.h"
#include "Entity.h"

/**
 * \brief Представляет из себя интерфейс, который может синхронизироваться с сервером
 */
class CSInterface
{
public:
	CSInterface(CSInterface** p, std::vector<InventoryContainer*> c);
	~CSInterface();
	size_t indexOfSlot(size_t container, size_t index) const;
	bool slotByIndex(InventoryContainer*& player, size_t index, InventoryContainer*& inv, size_t& id);
	void removePlayer(EntityPlayerMP* player);
	std::map<EntityPlayerMP*, InventoryContainer*> players;
	std::vector<InventoryContainer*> containers;
private:
	CSInterface** ptr = nullptr;
};