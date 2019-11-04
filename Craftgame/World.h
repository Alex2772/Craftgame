#pragma once

#include "Entity.h"
#include <vector>
#include <map>
#include "Chunk.h"
#include "def.h"
#include "WorldGenerator.h"
#include "ThreadUtils.h"
#include "LightSource.h"
#include "StandartFX.h"
#include "CustomFX.h"
#include <unordered_map>

typedef std::unordered_map<cindex, Chunk*> ChunksY;
typedef std::unordered_map<cindex, ChunksY> ChunksZ;
typedef std::unordered_map<cindex, ChunksZ> ChunksX;
typedef std::deque<Chunk*> Chunks;

class World {
private:
	WorldGenerator generator;
	TileEntity* __cached_tile = nullptr;
	//std::recursive_mutex mt;
public:
#ifndef SERVER
	LightSource l;
	Chunks renderList;
#endif
	bool isRemote = true;
	world_time time = 0;
	
	ChunksX chunkData;
	std::vector<Entity*> entities;
	std::deque<StandartFX*> particles;
	std::deque<CustomFX*> customParticles;
	
	World();
	~World();
	void spawnEntity(Entity* e, DPos p);
	void unload(Pos p);
	void removeEntity(Entity* entity);
	void loadChunk(Chunk* chunk);
	Entity* getEntity(size_t id);
	void setTime(world_time t);
	void playSound(std::string name, const DPos& pos);
	static Pos convertWS2CS(Pos p);
	static Pos convertWS2BS(Pos p);
	static Pos convertBS2WS(Pos c, Pos p);
	Chunk* getChunk(Pos p);
	Chunk* getChunkOrLoad(Pos p);
	Block* getBlock(Pos pos);
	TileEntity* getTileEntity(Pos pos);
	void updateBlock(Pos p);
	void updateBlocks(const Pos& pos);
	void setBlock(Block* x, Pos p);
	void setTileEntity(TileEntity* x, Pos p);
	void setBlockPython(Block*& x, Pos p);
	virtual void tick();
	Pos getHighestPoint(p_type x, p_type z);
	glm::vec3 calculateSunPos();

	bool checkCollision(const AABB& bb, Pos& res);
};
