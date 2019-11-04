#pragma once
#include "Item.h"
#include "ADT.h"

class ItemStack
{
private:
	friend class Item;
	Item* item;
	uint16_t count = 1;
	boost::shared_ptr<ADT::Map> adt = nullptr;
protected:
	ItemStack(Item* i);
public:
	virtual ~ItemStack();
	Item* getItem();
	uint16_t getCount();
	void setCount(uint16_t c);
	std::string getName();
	void setADT(boost::shared_ptr<ADT::Map> k);
	boost::shared_ptr<ADT::Map> getADT();
	boost::shared_ptr<ADT::Map> getRawADT() const;
};
