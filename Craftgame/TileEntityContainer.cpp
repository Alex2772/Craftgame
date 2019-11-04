#include "TileEntityContainer.h"
#include <boost/smart_ptr/make_shared.hpp>

TileEntityContainer::TileEntityContainer(Block* t):
TileEntity(t)
{

}

InventoryContainer* TileEntityContainer::getInventory()
{
	if (inv)
	{
		return inv;
	}
	inv = new InventoryContainer;
	static int width = 492;
	for (int x = 0; x < 9; x++)
	{
		inv->slots.push_back(new Slot(x * 52 + (width - 52 * 9) / 2, 20 + 52 * 0));
		inv->slots.push_back(new Slot(x * 52 + (width - 52 * 9) / 2, 20 + 52 * 1));
	}
	
	return inv;
}

TileEntityContainer::~TileEntityContainer()
{
	if (inv)
		delete inv;
	if (interface)
	{
		delete interface;
	}
}

CSInterface* TileEntityContainer::getInterface()
{
	if (interface)
	{
		return interface;
	}
	// TODO insert player's inventory
	interface = new CSInterface(&interface, {getInventory()});
	return interface;
}

boost::shared_ptr<ADT::Map> TileEntityContainer::serialize()
{
	if (inv) {
		boost::shared_ptr<ADT::Map> map = getADT();
		boost::shared_ptr<ADT::Value> v = boost::make_shared<ADT::Value>();
		ByteBuffer b;
		b << inv;
		v->setValueBuffer(b);
		map->putNode("container", v);
		return map;
	} else
	{
		return nullptr;
	}
}

void TileEntityContainer::deserialize(boost::shared_ptr<ADT::Map> data)
{
	boost::shared_ptr<ADT::Value> val = data->getNode("container")->toValue();
	if (val)
	{
		ByteBuffer b;
		val->toByteBuffer(b);
		b.seekg(0);
		InventoryContainer* asd = getInventory();
		b >> asd;
	}
}
