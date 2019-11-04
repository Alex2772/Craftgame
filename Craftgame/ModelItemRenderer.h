#pragma once
#include "IItemRenderer.h"
#include "AdvancedModel.h"
#include <string>

class ModelItemRenderer: public IItemRenderer
{
private:
	Material* mMaterial;
	AdvancedAssimpModel* mModel;
public:
	ModelItemRenderer(std::string model, Material* m);
	virtual void render(Type t, Item* item, ItemStack* stack);
};
