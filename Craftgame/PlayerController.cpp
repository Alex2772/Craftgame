#include "PlayerController.h"
#include "def.h"
#include "Block.h"
#include <glm/glm.hpp>
#include "GameEngine.h"
#include "Joystick.h"
#include "P07SetBlock.h"
#include "PClick.h"
#include "ItemCharging.h"


long long lastPlaceBlock;
extern float __swingAnim;
void PlayerController::leftClick()
{
#ifndef SERVER
	if (lastPlaceBlock < std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() && CGE::instance->guiScreens.empty()) {
		lastPlaceBlock = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() + 250;
		__swingAnim = 1.f;
		ItemStack* h = CGE::instance->thePlayer->getHeldItem();
		if (h && !h->getItem()->itemLeftClick(CGE::instance->thePlayer, h)) {
			return;
		}
		Block::BlockSide s;
		Pos p;
		if (CGE::instance->thePlayer->getBlockLookingAt(p, s))
			ts<IServer>::r(CGE::instance->server)->sendPacket(new PClick(0, &p, s));
		else
			ts<IServer>::r(CGE::instance->server)->sendPacket(new PClick(0, nullptr, s));
		return;
	}
	ts<IServer>::r(CGE::instance->server)->sendPacket(new PClick(0));
#endif
}
ItemStack* asd = nullptr;
void PlayerController::rightClick()
{
#ifndef SERVER
	if (!CGE::instance->guiScreens.empty())
		return;
	ItemStack* h = CGE::instance->thePlayer->getHeldItem();
	if (lastPlaceBlock < std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()) {
		lastPlaceBlock = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() + 250;
		if (h && !h->getItem()->itemRightClick(CGE::instance->thePlayer, h)) {
			return;
		}
		Block::BlockSide s;
		Pos p;
		Block* t = CGE::instance->thePlayer->getBlockLookingAt(p, s);
		ts<IServer>::r(CGE::instance->server)->sendPacket(new PClick(1, t ? &p : nullptr, s));
		if (t)
		{
			if (!t->onRightClick(p, CGE::instance->thePlayer))
			{
				return;
			}
		}
		if (h && dynamic_cast<Block*>(h->getItem()))
		{
			__swingAnim = 1.f;
		}
		if (IStackCharging* k = dynamic_cast<IStackCharging*>(h))
		{
			if (ItemCharging* x = dynamic_cast<ItemCharging*>(h->getItem())) {
				if (!k->isUsed)
				{
					ts<IServer>::rw lock(CGE::instance->server);
					asd = h;
					x->startCharging(h, CGE::instance->thePlayer);
				}
			}
		}
		return;
	}
	ts<IServer>::r(CGE::instance->server)->sendPacket(new PClick(1));
#endif
}
