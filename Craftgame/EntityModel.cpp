#include "EntityModel.h"
#include "GameEngine.h"

void EntityModel::render()
{
#ifndef SERVER
	EntitySkeleton::render();
	if (model) {
		//static Material* mat = CGE::instance->materialRegistry->getMaterial(_R("craftgame:gold"));
		//mat->bind();
		glDisable(GL_CULL_FACE);
		CGE::instance->renderer->setTransform(getTransform());

		std::map<std::string, glm::mat4> transformation;
		CGE::instance->renderer->renderModelAssimp(model, nullptr, transformation, true);
		glEnable(GL_CULL_FACE);
	}
#endif
}

EntityModel::EntityModel()
{
}

EntityModel::EntityModel(World* w, const size_t& _id) :
	EntitySkeleton(w, _id) {
}