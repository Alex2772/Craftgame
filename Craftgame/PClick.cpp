#include "PClick.h"
#include "TileEntity.h"
#include "GameEngine.h"


PClick::PClick(uint8_t b, Pos* p, Block::BlockSide s):
	PClick()
{
	button = b;
	if (p) {
		pos = *p;
		side = s;
	} else
	{
		button |= 0x80;
	}
}

PClick::PClick(uint8_t b) :
	PClick()
{
	button = b | 0x80;
}

void PClick::write(ByteBuffer& buffer)
{
	buffer << button;
	if (!(button & 0x80)) {
		buffer << pos;
		buffer << side;
	}
}

void PClick::read(ByteBuffer& buffer)
{
	buffer >> button;
	if (!(button & 0x80)) {
		buffer >> pos;
		buffer >> side;
	}
}

void PClick::onReceived()
{
}

void PClick::onReceived(EntityPlayerMP* player)
{
	if (player->getHeldItem())
	{
		switch (button & ~0x80) {
		case 0:
			if (!player->getHeldItem()->getItem()->itemLeftClick(player, player->getHeldItem()))
			{
				return;
			}
			break;
		case 1:
			if (!player->getHeldItem()->getItem()->itemRightClick(player, player->getHeldItem()))
			{
				return;
			}
			break;
		}
	}
	if (button & 0x80) {
		player->applyAnimation("right_swing", 1, false);
	} else if (glm::length(DPos(pos).toVec3() - player->getEyePos()) < 10.f) {
		
		Block* t = player->worldObj->getBlock(pos);
		if (t) {
			switch (button)
			{
			case 0:
				if (t->breakBlock(pos, player)) {
					CGE_EVENT("break_block", CGE_P("player", player)CGE_P("block", t)CGE_P("pos", &pos), [&]()
					{
						ts<IServer>::rw s(CGE::instance->server);
						player->worldObj->setBlock(CGE::instance->goRegistry->getBlock("blocks/air"), pos);
						player->applyAnimation("right_swing", 1, false);
					});
				}
				break;
			case 1:
				if (t->onRightClick(pos, player)) {
					if (player->getHeldItem() && player->getHeldItem()->getItem()->itemRightClick(player, player->getHeldItem())) {
						if (Block* block = dynamic_cast<Block*>(player->getHeldItem()->getItem())) {
							Pos p = pos;
							switch (side)
							{
							case Block::TOP:
								++p.y;
								break;
							case Block::BOTTOM:
								--p.y;
								break;
							case Block::LEFT:
								--p.x;
								break;
							case Block::RIGHT:
								++p.x;
								break;
							case Block::BACK:
								--p.z;
								break;
							default:
							case Block::FRONT:
								++p.z;
								break;
							}
							Block* n = player->worldObj->getBlock(p);
							if (n->isAir()) {
								if (block->onRightClick(p, player)) {
									CGE_EVENT("place_block", CGE_P("player", player)CGE_P("block", block)CGE_P("pos", &p), [&]()
									{
										player->worldObj->setBlock(block, p);
										player->applyAnimation("right_swing", 1, false);
									});
								}
							}
						}
					}
					break;
				}
			}
		}
	}
}
