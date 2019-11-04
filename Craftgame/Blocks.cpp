#include "GameEngine.h"
#include "Blocks.h"
#include "CraftgameServer.h"
#include "GuiInventory.h"
#include "TileEntityContainer.h"
#include "PContainerData.h"
#include "EntityItem.h"
#include "SolidBlock.h"
#ifndef SERVER
#include "TestFX.h"
#include "Random.h"
#endif 
void Blocks::init()
{
	CGE::instance->goRegistry->registerGO((new Block(_R("blocks/air"), 0))->setAir(true));
	CGE::instance->goRegistry->registerGO(new SolidBlock(_R("blocks/stone"), 1, 0, CGE::instance->materialRegistry->getMaterial("blocks/stone")));
	CGE::instance->goRegistry->registerGO(new BlockGrass);
	CGE::instance->goRegistry->registerGO(new SolidBlock(_R("blocks/dirt"), 3, 0, CGE::instance->materialRegistry->getMaterial("blocks/dirt")));
	CGE::instance->goRegistry->registerGO(new SolidBlock(_R("blocks/cobblestone"), 4, 0, CGE::instance->materialRegistry->getMaterial("blocks/cobblestone")));
	CGE::instance->goRegistry->registerGO(new BlockTorch);
	CGE::instance->goRegistry->registerGO(new SolidBlock(_R("blocks/gold"), 6, 0, CGE::instance->materialRegistry->getMaterial("gold")));
	CGE::instance->goRegistry->registerGO(new BlockRose);
	CGE::instance->goRegistry->registerGO(new BlockContainer);
}

BlockGrass::BlockGrass() :
	Block(_R("craftgame:blocks/grass"), 2, 0) {
}

TileEntity* BlockTorch::newTileEntity()
{
	return new TileEntity(this);
}

TileEntity* BlockRose::newTileEntity()
{
	return new TileEntity(this);
}
LightSource* BlockTorch::getLightSource()
{
	//return rand() % 2 == 0 ? new TorchLightSource(glm::vec3(0.5f, 0.8f, 0.5f)) : new LightSource(glm::vec3(0.5f, 0.8f, 0.5f), glm::vec3(float(rand() % 1000) / 100.f, float(rand() % 1000) / 100.f, float(rand() % 1000) / 100.f));
	return new TorchLightSource(glm::vec3(0.5f, 0.8f, 0.5f));
}

void BlockRose::renderBlock(const Pos& p, glm::mat4 m)
{
#ifndef SERVER
	static AdvancedAssimpModel* model = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel("rose"));
	static Material* material = CGE::instance->materialRegistry->getMaterial("rose");
	m = glm::translate(m, glm::vec3(-0.5f, 0, 0.5f));
	CGE::instance->renderer->setTransform(m);
	material->bind();
	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->color = glm::vec4(1.f);
	std::map<std::string, glm::mat4> tr;
	CGE::instance->renderer->renderModelAssimp(model, nullptr, tr, false);
#endif
}

void BlockRose::update(const Pos& k, World* w)
{
	Pos p = k;
	p.y -= 1;
	if (Block* b = w->getBlock(p))
	{
		if (b->getResource().path != "blocks/grass")
		{
			w->setBlock(CGE::instance->goRegistry->getBlock("blocks/air"), k);
		}
	}
}

bool BlockContainer::breakBlock(const Pos& p, EntityPlayerMP* player)
{
	TileEntity* t = player->worldObj->getTileEntity(p);
	if (TileEntityContainer* c = dynamic_cast<TileEntityContainer*>(t))
	{
		for (Slot*& s : c->getInventory()->slots)
		{
			if (s->stack)
			{
				EntityItem* e = new EntityItem(nullptr, CGE::instance->assignGlobalUniqueEntityId(), s->stack);
				
				player->worldObj->spawnEntity(e, t->getPos());
				e->motion.x = (rand() % 2000) / 1000.f - 1;
				e->motion.z = (rand() % 2000) / 1000.f - 1;
				e->motion.y = (rand() % 2000) / 1000.f;
				e->motion *= 10.f;
				s->stack = nullptr;
			}
		}
	}
	return true;
}

TileEntity* BlockContainer::newTileEntity()
{
	return new TileEntityContainer(this);
}

bool BlockContainer::onRightClick(const Pos& p, EntityPlayerMP* player)
{
	Block::onRightClick(p, player);
	TileEntityContainer* c = dynamic_cast<TileEntityContainer*>(player->worldObj->getTileEntity(p));
	player->openInterface(c->getInterface());
	return false;
}

void BlockTorch::renderBlock(const Pos& p, glm::mat4 m)
{
#ifndef SERVER
	static AdvancedAssimpModel* model = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel("torch"));
	static Material* material = CGE::instance->materialRegistry->getMaterial("surface");
	CGE::instance->renderer->setTransform(m);
	material->bind();
	CGE::instance->renderer->setTexturingMode(0);
	//std::map<std::string, SkeletalAnimation> temp;
	/*
	if (Keyboard::isKeyDown(Keyboard::LControl))
	{
	light->color = glm::vec3(float(rand() % 1000) / 100.f, float(rand() % 1000) / 100.f, float(rand() % 1000) / 100.f);
	}*/
	TileEntity* t = CGE::instance->thePlayer->worldObj->getTileEntity(p);
	CGE::instance->renderer->color = (glm::vec4(t ? t->light->color : glm::vec3(0.9f, 0.5f, 0), 1.f));
	std::map<std::string, glm::mat4> tr;
	CGE::instance->renderer->renderModelAssimpOnly(model, "1", nullptr, tr, false);
	CGE::instance->renderer->color = glm::vec4(1.f);
	CGE::instance->renderer->renderModelAssimpOnly(model, "0", nullptr, tr, false);
	if (t && !(rand() % 4)) {
		glm::vec3 v = (Random::nextVec3() * 2.f - 1.f) * 0.5f;
		v.y = .4f;
		glm::vec3 p = DPos(t->getPos()).toVec3() + 0.5f;
		p.y += .1f;
		CGE::instance->thePlayer->worldObj->particles.push_back(new TestFX(p, v));
	}
#endif
}

void BlockTorch::update(const Pos& k, World* w)
{
	Pos p = k;
	p.y -= 1;
	if (Block* b = w->getBlock(p))
	{
		if (!b->isSolid() || b->isAir())
		{
			w->setBlock(CGE::instance->goRegistry->getBlock(0, 0), k);
		}
	}
}
