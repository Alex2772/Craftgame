#include "TileEntity.h"
#include "GameEngine.h"
#include <boost/smart_ptr/make_shared.hpp>

TileEntity::TileEntity(Block* t):
	block(t)
{
	assert(block);
	if (block) {
		light = block->getLightSource();
		if (light)
		{
			lightOffset = light->pos;
		}
	}
}

TileEntity::TileEntity(const TileEntity& t):
	block(t.block),
	pos(t.pos),
	lightOffset(t.lightOffset)
{
	if (t.light)
	{
		light = new LightSource(*t.light);
	}
}

TileEntity::~TileEntity()
{
	if (light) {
		LightSource* l = light;
		CGE::instance->uiThread.push([l]()
		{
			delete l;
		});
	}
}

/**
 * \return Блок
 */
Block*& TileEntity::getBlock()
{
	return block;
}

void TileEntity::setPos(Pos p)
{
	pos = p;
	if (light)
		light->pos = DPos(pos).toVec3() + lightOffset;
}

const Pos& TileEntity::getPos()
{
	return pos;
}

boost::shared_ptr<ADT::Map> TileEntity::getADT()
{
	if (adt)
	{
		return adt;
	}
	return adt = boost::make_shared<ADT::Map>();
}

boost::shared_ptr<ADT::Map> TileEntity::serialize()
{
	return adt;
}

void TileEntity::deserialize(boost::shared_ptr<ADT::Map> data)
{
	adt = data;
}