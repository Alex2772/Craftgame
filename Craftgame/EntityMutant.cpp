#include "EntityMutant.h"
#include "GameEngine.h"
#include "CustomShader.h"
#include "SightFX.h"
#include "Random.h"
#include "PhysicsFX.h"

extern std::vector<BoneTransform> transforms;

void EntityMutant::render()
{
#ifndef SERVER
	static AdvancedAssimpModel* m = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel(_R("craftgame:dimon")));
	static Material* mat = CGE::instance->materialRegistry->getMaterial(_R("craftgame:entity/dimon/armor"));
	static Material* b = CGE::instance->materialRegistry->getMaterial(_R("craftgame:entity/dimon/body"));
	model = m;
	EntitySkeleton::render();
	updateAnimation("idle");
	std::map<std::string, glm::mat4> transformation;
	transformation["head"] = glm::rotate(glm::rotate(glm::mat4(1.0), glm::radians(yawHeadRender - yawRender), glm::vec3(0, 0.5, 0)), glm::radians(pitchRender), glm::vec3(0.5, 0, 0));
	CGE::instance->renderer->setTransform(glm::scale(glm::rotate(getTransform(), glm::radians(yawRender), glm::vec3(0, 0.5, 0)), glm::vec3(.01f)));
	mat->bind();
	CGE::instance->renderer->renderModelAssimpOnly(model, "mesh_head", &mAnimations, transformation, false);
	CGE::instance->renderer->renderModelAssimpOnly(model, "armor", &mAnimations, transformation, false);
	CustomShader::shader(CustomShader::SKELETON, "dimon1", "vec3 toCamera = normalize(viewPos - pass_Position); return mix(texture(material.tex, pass_texCoords + vec2(0, offset)).xyz + vec3(0.01f, 0.2f, 0.3f) * clamp(1 - dot(toCamera, normal), 0, 1), vec3(0.21f, 0.8f, 0.94f), clamp(dot(vec3(0, 1, 0), toCamera), 0, 1));", { {"offset", "float"} }, [&](boost::shared_ptr<Shader> shader)
	{
		b->bind();
		shader->loadFloat("offset", offset);
		CGE::instance->renderer->renderModelAssimpOnly(model, "kliki", &mAnimations, transformation, false);
		CGE::instance->renderer->renderModelAssimpOnly(model, "mesh_body", &mAnimations, transformation, false);
		offset += CGE::instance->millis * 0.4f;
		offset = fmodf(offset, 1.f);
	});
	
	CustomShader::shader(CustomShader::SKELETON, "dimon2", "vec3 toCamera = normalize(viewPos - pass_Position); return mix(vec3(0.01, 1, 1), vec3(0.4, 0.4, 0.4), clamp(dot(normal, toCamera), 0, 1));", {}, [&](boost::shared_ptr<Shader> shader)
	{
		CGE::instance->renderer->renderModelAssimpOnly(model, "eyes", &mAnimations, transformation, false);
	});
	model->nextFrame();
	ls.pos = (getPos() + DPos(0, 0.5, 0)).toVec3() + (Random::nextVec3() * 2.f - 1.f) * 0.1f;

	float x = Random::nextFloat() * 0.1f;
	float z = Random::nextFloat() * 0.1f;
	if (CGE::instance->renderer->renderType & GameRenderer::RENDER_DEFAULT) {
		CGE::instance->thePlayer->worldObj->particles.push_back(new SightFX(1, ls.pos, glm::vec3(x, -1, z), glm::vec4(0.2f, 0.82f, 0.91f, 0.4f)));
		if (rand() % 4 == 0)
			CGE::instance->thePlayer->worldObj->particles.push_back(new SightFX(2, ls.pos + glm::vec3(0, 1.2f, 0), glm::vec3(x, .2f, z), glm::vec4(0.2f, 0.82f, 0.91f, 0.4f)));
		std::vector<std::string> keks = { "right_hand", "left_hand" };
		for (auto& kek : keks) {
			if (rand() % 3 == 0) {
				glm::mat4 m(1.0);
				for (size_t i = 0; i < transforms.size(); i++)
				{
					if (transforms[i].name == kek)
					{
						m = transforms[i].m;
					}
				}
				glm::vec3 k = Random::nextVec3();
				k.x = k.x * 2.f - 1.f;
				k.z = k.z * 2.f - 1.f;
				k *= glm::vec3(0.2f, 0.05f, 0.2f);
				CGE::instance->thePlayer->worldObj->particles.push_back(new PhysicsFX(2, glm::vec3(glm::scale(glm::rotate(getTransform(), glm::radians(yawRender), glm::vec3(0, 0.5, 0)), glm::vec3(.01f)) * m * glm::vec4(0, 0.15f, 0, 1)) + k, glm::vec3(x, .2f, z), glm::vec4(0.2f, 0.82f, 0.91f, 0.4f), 1.f, 3.f));
			}
		}
	}
#endif
}

void EntityMutant::tick()
{
	EntitySkeleton::tick();
	if (!worldObj->isRemote && !isOnGround())
	{
		motion.y -= CGE::instance->millis * 9.8f;
	}
}
