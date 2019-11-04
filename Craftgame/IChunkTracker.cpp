#include "IChunkTracker.h"
#include "Chunk.h"
#include <algorithm>
#include "GameEngine.h"

IChunkTracker::~IChunkTracker()
{
	for (size_t i = mTrackedChunks.size(); i > 0 ; i--)
	{
		unlink(mTrackedChunks[i - 1], this);
	}
}

bool IChunkTracker::isLinked(Chunk* c, IChunkTracker* t)
{
	return std::find(c->mChunkTrackers.begin(), c->mChunkTrackers.end(), t) != c->mChunkTrackers.end();
}

/**
 * \brief Разрывает связь между объектом-трекером и чанком. Если у чанка трекеров больше не остаётся, он отгружается.
 * \param c Чанк
 * \param t Объект-трекер
 */
void IChunkTracker::unlink(Chunk* c, IChunkTracker* t)
{
	c->mChunkTrackers.erase(std::find(c->mChunkTrackers.begin(), c->mChunkTrackers.end(), t));
	t->mTrackedChunks.erase(std::find(t->mTrackedChunks.begin(), t->mTrackedChunks.end(), c));

	if (c->mChunkTrackers.empty())
	{
		ts<IServer>::rw lock(CGE::instance->server);
		c->unload();
	}
}

void IChunkTracker::link(Chunk* c, IChunkTracker* t)
{
	c->mChunkTrackers.push_back(t);
	t->mTrackedChunks.push_back(c);
}
