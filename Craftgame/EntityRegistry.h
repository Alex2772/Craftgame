#pragma once

#include <vector>
#include "Entity.h"
#include "Instancer.h"

class EntityRegistry
{
private:
	std::vector<InstancerBase*> entities;
public:
	EntityRegistry();
	~EntityRegistry();
	void registerEntity(InstancerBase* cmd);
	template<class T>
	void registerEntity()
	{
		registerEntity(new Instancer<T>);
	}
	Entity* instanceEntity(const size_t tid, World* w, const id_type i);
};