#pragma once
#include "IItemRenderer.h"
#include "AdvancedModel.h"

class FbxModelItemRenderer : public IItemRenderer
{
protected:
	Material* mMaterial;
	AdvancedAssimpModel* mModel;
public:
	FbxModelItemRenderer(std::string model, Material* m);
	virtual void render(Type t, Item* item, ItemStack* stack);
};
