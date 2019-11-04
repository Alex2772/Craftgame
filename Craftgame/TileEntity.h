#pragma once

#include "def.h"
#include "Block.h"
#include "Chunk.h"
#include "LightSource.h"
#include "ADT.h"

class ChunkPartHandler;

class TileEntity
{
private:
	Pos pos;
	Block* block;
	glm::vec3 lightOffset;
	boost::shared_ptr<ADT::Map> adt;
public:
	LightSource* light;
	TileEntity(Block* t);
	TileEntity(const TileEntity& t);
	virtual ~TileEntity();
	Block*& getBlock();
	void setPos(Pos p);
	const Pos& getPos();
	boost::shared_ptr<ADT::Map> getADT();
	virtual boost::shared_ptr<ADT::Map> serialize();
	virtual void deserialize(boost::shared_ptr<ADT::Map> data);
};

