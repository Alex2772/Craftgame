#include "ItemStack.h"
#include "GameEngine.h"
#include <boost/smart_ptr/make_shared.hpp>


ItemStack::ItemStack(Item* i):
item(i)
{
}
extern ItemStack* asd;
ItemStack::~ItemStack()
{
	if (asd == this)
	{
		asd = nullptr;
	}
}

Item* ItemStack::getItem()
{
	return item;
}

uint16_t ItemStack::getCount()
{
	return count;
}

void ItemStack::setCount(uint16_t c)
{
	if (c > item->getMaxStackCount())
		c = item->getMaxStackCount();
	count = c;
}

std::string ItemStack::getName()
{
	// TODO ADT custom name
	return CGE::instance->currentLocale->localize(item->getResource().full);
}

void ItemStack::setADT(boost::shared_ptr<ADT::Map> k)
{
	adt = k;
}
boost::shared_ptr<ADT::Map> ItemStack::getRawADT() const
{
	return adt;
}

boost::shared_ptr<ADT::Map> ItemStack::getADT()
{
	if (!adt)
	{
		adt = boost::make_shared<ADT::Map>();
	}
	return adt;
}
