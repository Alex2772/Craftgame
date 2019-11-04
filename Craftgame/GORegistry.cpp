#include "GORegistry.h"
#include "CraftgameException.h"

GORegistry::GORegistry()
{
}

void GORegistry::registerGO(Item* b)
{
	data[b->getResource()] = b;
	if (iddata.size() <= b->getId())
	{
		iddata.resize(b->getId() + 1);
	}
	std::vector<Item*>& m = iddata[b->getId()];
	
	if (m.size() <= b->getData())
	{
		m.resize(b->getData() + 1);
	} else
	{
		Item* s = m[b->getData()];
		if (s)
		{
			CraftgameException c(this, std::string("An attempt made to replace ") + std::to_string(s->getId()) + " " + std::to_string(s->getData()) + " (" + s->getResource().full + ") with " + b->getResource().full);
			c.addCI(new CrashInfo(b));
			c.addCI(new CrashInfo(s));
			throw c;
		}
	}
	m[b->getData()] = b;
}

Block* GORegistry::getBlock(bid id, unsigned char d)
{
	Item* g = getGO(id, d);
	return dynamic_cast<Block*>(g);
}

Item* GORegistry::getGO(_R r)
{
	bdata::iterator f = data.find(r);
	if (f != data.end())
	{
		return f->second;
	}
	return nullptr;
}

Block* GORegistry::getBlock(_R r)
{
	Item* g = getGO(r);
	return dynamic_cast<Block*>(g);
}

Item* GORegistry::getGO(bid id, unsigned char d)
{
	if (iddata.size() > id && iddata[id].size() > d)
	{
		return iddata[id][d];
	}
	return nullptr;
}
