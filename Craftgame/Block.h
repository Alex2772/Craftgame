#pragma once

#include "global.h"
#include "Material.h"
#include "def.h"
#include "LightSource.h"
#include "Item.h"

class TileEntity;
class World;
class EntityPlayerMP;
class ChunkPartHandler;
struct BlockFaces;

class Block: public Item, IItemRenderer {
private:
	bool air = false;
	bool solid = true;
	bool opaque = true;
protected:
	Material* material = nullptr;
public:
	enum BlockSide
	{
		BOTTOM = 0,
		TOP = 1,
		LEFT = 2,
		RIGHT = 3,
		BACK = 4,
		FRONT = 5
	};

	Block(_R r, bid id, byte data = 0);
	virtual ~Block() = default;
	virtual Material* getMaterialBySide(BlockSide side);
	virtual bool isAir();

	/**
	 * \brief Определяет газ. Если true, параметры solid и opaque становятся false 
	 */
	Block* setAir(bool a)
	{
		air = a;
		if (a) {
			solid = false;
			opaque = false;
		}
		return this;
	}

	virtual bool isSolid();
	virtual bool isOpaque();
	virtual void update(const Pos& e, World* w);
	virtual LightSource* getLightSource() { return nullptr; }
	virtual TileEntity* newTileEntity();
	virtual void render(Type t, Item* item, ItemStack* stack) override;
	virtual bool onRightClick(const Pos& t, EntityPlayerMP* player);
	virtual bool breakBlock(const Pos& block, EntityPlayerMP* player);
	virtual bool placeBlock(const Pos& pos, EntityPlayerMP* player);
	virtual bool itemRightClick(EntityPlayerMP* sabj, ItemStack* stack) override;
	virtual void mesh(const Pos& pos, ChunkPartHandler* mesher, const BlockFaces& bf);
};
