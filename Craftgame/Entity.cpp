#include "Entity.h"

Entity::Entity(World* w, const size_t& _id) :
	id(_id),
	worldObj(w)
{

}
const size_t& Entity::getId() {
	return id;
}