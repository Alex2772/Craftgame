
#include "FbxModelItemRenderer.h"
#include "GameEngine.h"
#include "IAnimatable.h"

FbxModelItemRenderer::FbxModelItemRenderer(std::string model, Material* m) :
	mMaterial(m)
{
	mModel = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->registerModel(std::string("res/models/") + model, model, false));
}

void FbxModelItemRenderer::render(Type t, Item* item, ItemStack* stack)
{
#ifndef SERVER
	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->setColor(glm::vec4(1.f));
	mMaterial->bind();
	std::map<std::string, glm::mat4> tr;
	std::map<std::string, SkeletalAnimation> kek;
	
	if (IAnimatable* k = dynamic_cast<IAnimatable*>(stack)) {
		
		std::map<std::string, SkeletalAnimation>* sk = nullptr;
		k->animate(sk, tr);
		CGE::instance->renderer->setTransform(glm::scale(CGE::instance->renderer->transform, glm::vec3(.02f)));
		
		if (t == FIRST_PERSON) {
			int x = CGE::instance->renderer->renderType;
			CGE::instance->renderer->renderType |= GameRenderer::RENDER_FIRST;

			CGE::instance->renderer->renderModelAssimp(mModel, sk, tr, false);
			CGE::instance->renderer->renderType = x;
		} else 
		{
			CGE::instance->renderer->renderModelAssimp(mModel, sk, tr, false);
		}
	}
	else
	{
		CGE::instance->renderer->renderModelAssimp(mModel, nullptr, tr, false);
	}
#endif
}