#include "EntityPlayer.h"
#include "GameEngine.h"
#include "Slot.h"
#include "GuiSeekBar.h"
#include <boost/smart_ptr/make_shared.hpp>

//#define MODEL_DEBUG
extern std::vector<BoneTransform> transforms;

void EntityPlayer::render()
{
#ifndef SERVER
	bool g = false;
	{
		char d = gameProfile.getUsername()[gameProfile.getUsername().length() - 1];
		switch (d)
		{
		case 'A':
		case 'a':
			static AdvancedAssimpModel* m = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel(_R("craftgame:playerg")));
			model = m;
			g = true;
		}
	}
	updateAnimation("idle");
	if (isOnGround())
	{
		updateAnimation("fall", 0);
	} else
	{
		updateAnimation("fall");
	}
	if (this->getHeldItem())
	{
		updateAnimation("right_hold");
	} else
	{
		updateAnimation("right_hold", 0);
	}
	EntitySkeleton::render();
	static Material* mat = CGE::instance->materialRegistry->getMaterial(_R("craftgame:skin"));
	if (!skin) {
		skin = CGE::instance->skinManager.getSkin(gameProfile.getUsername());
	}
	size_t prev = mat->texture;
	mat->texture = *skin;
	mat->bind();
	glDisable(GL_CULL_FACE);
	CGE::instance->renderer->setTexturingMode(1);
	std::map<std::string, glm::mat4> transformation;
#ifdef MODEL_DEBUG
	CGE::instance->renderer->setTransform(glm::scale(glm::rotate(getTransform(), glm::radians(yawRender + 180), glm::vec3(0, 0.5, 0)), glm::vec3(0.1f)));
	transformation["Head"] = glm::rotate(glm::rotate(glm::mat4(1.0), glm::radians(yawHeadRender - yawRender), glm::vec3(0, 0.5, 0)), glm::radians(-pitchRender), glm::vec3(0.5, 0, 0));
	CGE::instance->renderer->renderModelAssimp(model, nullptr, transformation, true);
#else
	CGE::instance->renderer->setTransform(glm::scale(glm::rotate(getTransform(), glm::radians(yawRender), glm::vec3(0, 0.5, 0)), glm::vec3(g ? 0.000292f : 0.0003f)));
	transformation["head"] = glm::rotate(glm::rotate(glm::mat4(1.0), glm::radians(yawHeadRender - yawRender), glm::vec3(0, 0.5, 0)), glm::radians(pitchRender), glm::vec3(0.5, 0, 0));
	CGE::instance->renderer->renderModelAssimp(model, &mAnimations, transformation, false);
#endif
	model->nextFrame();
	mat->texture = prev;
	glEnable(GL_CULL_FACE);
	if (ItemStack* k = this->getHeldItem())
	{
		bool isBlock = dynamic_cast<Block*>(k->getItem());
		if (isBlock)
		{
			updateAnimation("right_hold_block").speed = 10.f;
		} else
		{
			updateAnimation("right_hold_block", 0);
		}
		glm::mat4 m(1.0);
		for (size_t i = 0; i < transforms.size(); i++)
		{
			if (transforms[i].name == "right.005")
			{
				m = glm::scale(glm::mat4(1.0), glm::vec3(1.f / 330.f / 10.f)) * (transformation[transforms[i].name] * transforms[i].m * transforms[i].o) * glm::scale(glm::mat4(1.0), glm::vec3(1.f, 1.f, 1.f / 3.f) * 2.f) * glm::translate(glm::mat4(1.0), glm::vec3(0, -0.33f, -1.92f));
				if (isBlock)
					m *= glm::rotate(glm::mat4(1.0), glm::radians(70.f), glm::vec3(1, 1, 1));
			}
		}
		
		if (!isBlock)
		{
			m = glm::translate(m, glm::vec3(0, 0, 0.4f));
			m = glm::rotate(m, glm::radians(90.f), glm::vec3(0, 1.f, 0));
		}
		CGE::instance->renderer->setTransform(armTransform = glm::rotate(getTransform(), glm::radians(yawRender), glm::vec3(0, 0.5, 0)) * m);
		k->getItem()->getRenderer()->render(IItemRenderer::ENTITY, k->getItem(), k);
	} else {
		updateAnimation("right_hold_block", 0);
	}
	if (CGE::instance->renderer->renderType & GameRenderer::RENDER_DEFAULT && CGE::instance->thePlayer != this) {
		if (glm::length(CGE::instance->thePlayer->getPos().toVec3() - posRender.toVec3()) <= 16)
		{
			CGE::instance->postRender([&]() {
				glEnable(GL_DEPTH_TEST);
				glm::vec3 tr = posRender.toVec3();
				//tr.x += aabb.getWidthX() / 2;
				//tr.z += aabb.getWidthZ() / 2;
				tr.y += aabb.getHeight() + 0.4f;
				glm::mat4 view;
				view = glm::rotate(view, glm::radians(CGE::instance->camera->yaw), glm::vec3(0, 1, 0));
				view = glm::rotate(view, glm::radians(CGE::instance->camera->pitch), glm::vec3(1, 0, 0));
				glm::mat4 str = CGE::instance->camera->createViewMatrix() * glm::scale(glm::translate(glm::mat4(1.0), tr), glm::vec3(.01f)) * view;
				FontStyle fs;
				fs.size = 22;
				static CFont* font = CGE::instance->fontRegistry->getCFont("default");
				int width = font->length(gameProfile.getUsername(), fs.size) + 10;
				CGE::instance->renderer->setGui3D(true);
				CGE::instance->renderer->setMaskingMode(0);
				for (byte i = 0; i < 2; i++) {
					if (i == 1)
					{
						glDisable(GL_DEPTH_TEST);
						fs.color = glm::vec4(1, 1, 1, 0.2f);
					}
					CGE::instance->renderer->setTransform(str);
					CGE::instance->renderer->setTexturingMode(0);
					CGE::instance->renderer->setColor(i == 1 ? glm::vec4(0, 0, 0, 0.15f) : glm::vec4(0, 0, 0, 0.5f));
					CGE::instance->renderer->drawRect(width / -2, 0, width, fs.size + 4);
					CGE::instance->renderer->setColor(glm::vec4(1));
					CGE::instance->renderer->setTransform(glm::translate(str, glm::vec3(0, 0, 0.1f)));
					CGE::instance->renderer->drawString(0, -2, gameProfile.getUsername(), Align::CENTER, fs);
					str = glm::translate(str, glm::vec3(0, 0, 0.1f));
				}
				CGE::instance->renderer->setGui3D(false);
			});
		}
	}
#endif
}

EntityPlayer::EntityPlayer():
gameProfile("Unknown")
{
#ifndef SERVER
#ifdef MODEL_DEBUG
	model = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel(_R("craftgame:debug")));
#else
	model = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel(_R("craftgame:player")));
#endif
#endif
	adtTag->putNode("name", boost::make_shared<ADT::ADTString>(gameProfile.getUsername()));
	weight = 80.f;
}

EntityPlayer::EntityPlayer(World* w, const size_t& _id, GameProfile& _g) :
	EntitySkeleton(w, _id),
	gameProfile(_g) {
#ifndef SERVER
#ifdef MODEL_DEBUG
	model = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel(_R("craftgame:debug")));
#else
	model = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel(_R("craftgame:player")));
#endif
#endif
	adtTag->putNode("name", boost::make_shared<ADT::ADTString>(gameProfile.getUsername()));
	weight = 80.f;
}

EntityPlayer::~EntityPlayer()
{
	if (inventory)
	{
		delete inventory;
	}
}

const GameProfile& EntityPlayer::getGameProfile() {
	return gameProfile;
}

void EntityPlayer::unpack()
{
	Entity::unpack();
	if (boost::shared_ptr<ADT::Node> n = adtTag->getNode("name"))
		gameProfile = GameProfile(n->toString());
}

byte EntityPlayer::getItemIndex()
{
	return itemIndex;
}

void EntityPlayer::setItemIndex(byte index)
{
	itemIndex = index;
}

ItemStack* EntityPlayer::getHeldItem()
{
	return nullptr;
}

void EntityPlayer::tick()
{
	EntitySkeleton::tick();
	if (!worldObj->isRemote && !isOnGround() && !isFlying())
	{
		motion.y -= CGE::instance->millis * 30.f;
	}
}
