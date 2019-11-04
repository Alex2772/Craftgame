#pragma once
#include "Block.h"

class SolidBlock: public Block
{
public:
	SolidBlock(_R _r, bid _id, byte _data, Material* m):
		Block(_r, _id, _data)
	{
		material = m;
	}
};
