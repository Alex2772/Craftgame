#pragma once
#include  <vector>

class Block;
class Chunk;
class Pos;

/**
 * \brief Интерфейс, "следящий" за чанком.
 */
class IChunkTracker
{
public:
	std::vector<Chunk*> mTrackedChunks;
	virtual ~IChunkTracker();
	virtual void setBlock(Block* te, const Pos& p) = 0;
	static bool isLinked(Chunk* c, IChunkTracker* t);

	static void unlink(Chunk* c, IChunkTracker* t);
	static void link(Chunk* c, IChunkTracker* t);
};