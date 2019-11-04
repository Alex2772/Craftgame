#pragma once

#include "Block.h"
#include <map>

class GORegistry
{
private:
	typedef std::vector<std::vector<Item*>> biddata;
	biddata iddata;
public:
	typedef std::map<_R, Item*> bdata;
	bdata data;
	GORegistry();
	void registerGO(Item* b);
	Item* getGO(_R r);
	Block* getBlock(_R r);
	Item* getGO(bid id, unsigned char d = 0);
	Block* getBlock(bid id, unsigned char d = 0);
};
