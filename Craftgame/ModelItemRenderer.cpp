
#include "ModelItemRenderer.h"
#include "GameEngine.h"

ModelItemRenderer::ModelItemRenderer(std::string model, Material* m) :
	mMaterial(m)
{
	mModel = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->registerModel(std::string("res/models/") + model, model, false));
}

void ModelItemRenderer::render(Type t, Item* item, ItemStack* stack)
{
#ifndef SERVER
	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->color = glm::vec4(1.f);
	mMaterial->bind();
	std::map<std::string, glm::mat4> tr;
	CGE::instance->renderer->renderModelAssimp(mModel, nullptr, tr, false);
#endif
}