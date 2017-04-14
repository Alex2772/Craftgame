#include "World.h"

World::World() {

}
World::~World() {
	for (size_t i = 0; i < entities.size(); i++) {
		delete entities[i];
	}
	entities.clear();
}