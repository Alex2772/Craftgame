#pragma once
#include "Material.h"

class MaterialRegistry {
private:
	std::vector<Material*> materials;
public:
	MaterialRegistry();
	~MaterialRegistry();
	void registerMaterial(Material* material);
	Material* getMaterial(_R resource);
};