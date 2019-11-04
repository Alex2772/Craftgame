#pragma once
#include "BlockCustomRender.h"
#include "Utils.h"
#include "TorchLightSource.h"

namespace Blocks
{
	void init();
}

class BlockGrass : public Block {
public:
	BlockGrass();
	virtual Material* getMaterialBySide(BlockSide side) {
		static Material* d = CGE::instance->materialRegistry->getMaterial(_R("craftgame:blocks/dirt"));
		static Material* t = CGE::instance->materialRegistry->getMaterial(_R("craftgame:blocks/grass/top"));
		static Material* b = CGE::instance->materialRegistry->getMaterial(_R("craftgame:blocks/grass/side"));
		switch (side) {
		case TOP:
			return t;
		case BOTTOM:
			return d;
		default:
			return b;
		}
	}
};

class BlockTorch: public BlockCustomRender
{
public:
	BlockTorch(): BlockCustomRender(_R("craftgame:blocks/torch"), 5, 0) {};

	virtual Material* getMaterialBySide(BlockSide side) {
		static Material* d = CGE::instance->materialRegistry->getMaterial(_R("craftgame:blocks/dirt"));
		return d;
	}

	virtual void renderBlock(const Pos& p, glm::mat4) override;

	virtual bool isSolid() override {
		return false;
	}
	virtual bool isOpaque() override {
		return false;
	}
	virtual void update(const Pos& p, World* w);
	virtual TileEntity* newTileEntity() override;

	virtual LightSource* getLightSource();
};
class BlockRose : public BlockCustomRender
{
public:
	BlockRose() : BlockCustomRender(_R("craftgame:blocks/rose"), 7, 0) {};

	virtual Material* getMaterialBySide(BlockSide side) {
		static Material* d = CGE::instance->materialRegistry->getMaterial(_R("craftgame:blocks/dirt"));
		return d;
	}

	virtual TileEntity* newTileEntity() override;
	virtual void renderBlock(const Pos& p, glm::mat4) override;

	virtual bool isSolid() override {
		return false;
	}
	virtual bool isOpaque() override {
		return false;
	}
	virtual void update(const Pos& p, World* w);
};

class BlockContainer: public Block
{
public:
	BlockContainer(): Block("blocks/container", 9, 0) {}
	virtual Material* getMaterialBySide(BlockSide side) {
		static Material* d = CGE::instance->materialRegistry->getMaterial(_R("craftgame:blocks/container"));
		return d;
	}
	virtual bool breakBlock(const Pos& p, EntityPlayerMP* player);

	virtual TileEntity* newTileEntity() override;
	virtual bool onRightClick(const Pos& p, EntityPlayerMP* player) override;
};