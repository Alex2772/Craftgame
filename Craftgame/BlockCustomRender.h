#pragma once
#include "Block.h"

class BlockCustomRender : public Block {
public:
	BlockCustomRender(const _R& r, bid id, byte data)
		: Block(r, id, data)
	{
	}

	virtual void renderBlock(const Pos& p, glm::mat4) {};
};