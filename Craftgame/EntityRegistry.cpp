#include "EntityRegistry.h"


EntityRegistry::EntityRegistry()
{
}

EntityRegistry::~EntityRegistry() {
	for (size_t i = 0; i < entities.size(); i++)
		delete entities[i];
	entities.clear();
}

void EntityRegistry::registerEntity(InstancerBase* e) {
	entities.push_back(e);
}


/**
* \brief Создаёт инстанцию энтити с ID p
* \param p ID
* \return Инстанция энтити
*/
Entity* EntityRegistry::instanceEntity(const size_t tid, World* w, const id_type i)
{
	if (tid < entities.size())
	{
		Entity* e = reinterpret_cast<Entity*>(entities[tid]->create());
		e->worldObj = w;
		e->setId(i);
		return e;
	}
	return nullptr;
}