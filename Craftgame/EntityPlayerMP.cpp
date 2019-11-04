#include "EntityPlayerMP.h"
#include "P04ServerData.h"
#include "GameEngine.h"
#include "P05ChunkData.h"
#include "PEntityMove.h"
#include "P07SetBlock.h"
#include "PContainerData.h"
#include "InventoryPlayerContainer.h"
#include "ItemCharging.h"
#include "PForce.h"

EntityPlayerMP::EntityPlayerMP()
{
	
}

EntityPlayerMP::EntityPlayerMP(World* w, const size_t& _id, GameProfile _g) :
	EntityPlayer(w, _id, _g),
	netHandler(nullptr)
{
	inventory = new InventoryPlayerContainer(this);

	static int width = 492;
	static int height = 400;
	for (int x = 0; x < 9; x++)
	{
		inventory->slots.push_back(new Slot(x * 52 + (width - 52 * 9) / 2, height - 52 - 10));
	}
	for (int y = 2; y >= 0; y--) {
		for (int x = 0; x < 9; x++)
		{
			inventory->slots.push_back(new Slot(x * 52 + (width - 52 * 9) / 2, height - 52 * (y + 2) - 18));
		}
	}
	inventory->slots[0]->stack = CGE::instance->goRegistry->getGO(_R("items/yoyo"))->newStack();
	inventory->slots[0]->stack->setCount(1);
	for (size_t i = 0; i < 10; i++) {
		int slot = 0;
		do {
			slot = rand() % inventory->slots.size();
		} while (inventory->slots[slot]->stack);
		inventory->slots[slot]->stack = CGE::instance->goRegistry->getGO(rand() % 8 + 1, 0)->newStack();
		inventory->slots[slot]->stack->setCount(rand() % 64 + 1);
	}
}

EntityPlayerMP::~EntityPlayerMP()
{
}

void EntityPlayerMP::setPos(const DPos& p)
{
	EntityPlayer::setPos(p);
	if (!worldObj->isRemote) {
		selectChunks();
	}
	//pos = p;
	//motion = glm::vec3(0);
}
void EntityPlayerMP::setPosNonServer(const DPos& p)
{
	pos = p;
	motion = glm::vec3(0);
}
/**
 * \brief Ищет чанки вокруг себя и отправляет их.
 */
void EntityPlayerMP::selectChunks()
{
	if (!this)
		return;
	const int playerView = 4; // "Радиус"
	Pos rpos(pos);
	Pos playerChunkPos = World::convertWS2CS(rpos);
	std::function<void(Pos)> ololo = [&](Pos p)
	{
		Pos sub = playerChunkPos + p;
		if (sub.y < 0)
			return;
		Chunk* c = worldObj->getChunkOrLoad(sub);
		if (!isLinked(c, this)) {
			link(c, this);
			netHandler->sendPacket(new P05ChunkData(c));
		}
	};
	for (p_type x = 0; x <= playerView; x++)
	{
		for (p_type z = 0; z <= playerView; z++)
		{
			for (p_type y = 0; y <= playerView; y++)
			{
				ololo({ x, y, z });
				ololo({ -x, y, -z });
				ololo({ x, y, -z });
				ololo({ -x, y, z });
				ololo({ x, -y, z });
				ololo({ -x, -y, -z });
				ololo({ x, -y, -z });
				ololo({ -x, -y, z });;
			}
		}
	}
}
#ifndef SERVER
#include "GuiInventory.h"
#endif

void EntityPlayerMP::openInterface(CSInterface* i)
{
	if (!interface) {
		interface = i;
		i->players[this] = inventory;
		if (!worldObj->isRemote)
		{
			netHandler->sendPacket(new PContainerData(this, interface));
		}

#ifndef SERVER
		CGE::instance->displayGuiScreen(new GuiInventory(i->containers));
#endif
	}
}

void EntityPlayerMP::updateState(char newState)
{
	state = newState;
	ts<IServer>::r s(CGE::instance->server);
	if (CGE::instance->server) {
		P04ServerData* p = new P04ServerData(3, nullptr);
		p->state = state;
		s->sendPacket(p);
	}
}

ItemStack* EntityPlayerMP::getHeldItem()
{
	return inventory->slots[getItemIndex()]->stack;
}

void EntityPlayerMP::setBlock(Block* te, const Pos& p)
{
	if (!worldObj->isRemote)
	{
		netHandler->sendPacket(new P07SetBlock(te, p));
	}
}

void EntityPlayerMP::tick()
{
	/*
	if (!worldObj->isRemote && inAirTicks > 40 && prevPos.y - pos.y < 0.1f && !isFlying())
	{
		getNetHandler()->disconnect("U ar not flying, blet");
	}*/
	if (!isFlying() && !isOnGround())
	{
		selfMotion.y = 0.f;
	} else if (glm::abs(selfMotion.y) > 0.1f)
	{
		float speed = 15.f;
		if (isFlying())
		{
			speed = 2.f;
		}
		selfMotion.y = speed * glm::sign(selfMotion.y);
	}
	const float speed = (isFlying() || isOnGround()) ? 2.f : 0.05f;
	applyMotion(glm::vec3(selfMotion.x * speed, selfMotion.y, selfMotion.z * speed));
	if (isFlying() && isOnGround())
	{
		setFlying(false);
	}
	if (itemCharging && this->getHeldItem() != itemCharging)
	{
		IStackCharging* k = dynamic_cast<IStackCharging*>(itemCharging);
		if (k->isUsed)
		{
			dynamic_cast<ItemCharging*>(itemCharging->getItem())->stopCharging(itemCharging, this);
		}
	}
	if (this->getHeldItem())
	{
		this->getHeldItem()->getItem()->tick(this->getHeldItem(), this);
	}
	DPos prev = pos;
	EntityPlayer::tick();
}
void EntityPlayerMP::applyMotion(glm::vec3 m)
{
	EntityPlayer::applyMotion(m);
	/*
	if (!worldObj->isRemote && m != glm::vec3(0.f) && glm::length(m) > 0.001f)
	{
		//pos += m;
		//getNetHandler()->sendPacket(new PForce(m));
	} else
	{
		//EntityPlayer::applyMotion(m);
	}*/
}

bool __collision(Entity* entity, AABB& bb, World* worldObj, DPos& pos, float& mX, float& mY, float& mZ);
bool EntityPlayerMP::performCollisionChecks()
{
	/*
	bool ret = false;
	DPos p = pos;
	glm::vec3 prevM = motion;
	p.y += motion.y * CGE::instance->millis;
	float kek = 0;
	float kek1 = 0;
	ret |= __collision(this, aabb, worldObj, p, kek, motion.y, kek1);

	p.x += motion.x * CGE::instance->millis;
	kek = 0;
	kek1 = 0;
	ret |= __collision(this, aabb, worldObj, p, motion.x, kek, kek1);

	p.z += motion.z * CGE::instance->millis;
	kek = 0;
	kek1 = 0;
	ret |= __collision(this, aabb, worldObj, p, kek, kek1, motion.z);

	motion *= (isOnGround() ? glm::vec3(0.8f) : glm::vec3(1.f) / (glm::max)(glm::abs(motion) / 100.f, 1.01f));

	if (p != pos) {
		pos = p;
		this->onVelocityChanged(motion - prevM);
	}
	return ret;*/
	if (isFlying())
		applyMotionNonServer(-motion * 0.1f);
	return Entity::performCollisionChecks();
}
