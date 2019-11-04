#include "MaterialRegistry.h"

MaterialRegistry::MaterialRegistry() {
}
MaterialRegistry::~MaterialRegistry() {
	for (int i = 0; i < materials.size(); i++)
		delete materials[i];
	materials.clear();
}
#include "GameEngine.h"
void MaterialRegistry::registerMaterial(Material* material) {
	materials.push_back(material);
	CGE::instance->logger->info("Registered material " + material->r.full);
}
Material* MaterialRegistry::getMaterial(_R resource) {
	static Material* s = nullptr;
	if (s != nullptr && s->r.full == resource.full)
		return s;
	for (std::vector<Material*>::iterator i = materials.begin(); i != materials.end(); i++) {
		if ((*i)->r.full == resource.full) {
			s = *i;
			return *i;
		}
	}
	return nullptr;
}