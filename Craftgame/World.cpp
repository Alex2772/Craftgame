#include "World.h"
#include "EntityPlayerMP.h"
#include "GameEngine.h"
#include "OS.h"
#include "Models.h"
#include "P07SetBlock.h"
#include "PSpawnEntity.h"
#include "PDespawnEntity.h"
#include "PEntityMove.h"
#include "PEntityLook.h"
#include "EntityModel.h"
#include "File.h"
#include "Joystick.h"
#include "PWorldTime.h"
#include "PContainerData.h"
#include "PSound.h"
#include "PItemHeld.h"

Chunk* __cached_chunk = nullptr;

World::World()
#ifndef SERVER
	:
	l(glm::vec3(0), glm::vec3(1))
#endif
{
#ifdef SERVER
	isRemote = false;
#else
	CGE::instance->camera->pos.y = 52;
#endif
	generator.worldObj = this;

#ifndef SERVER
	//EntityModel* sk = new EntityModel(this, 228);
	//sk->model = dynamic_cast<AdvancedAssimpModel*>(CGE::instance->modelRegistry->getModel("debug"));
	//spawnEntity(sk, DPos(0, 52, 0));
#endif
}
World::~World() {
	__cached_chunk = nullptr;
	for (size_t i = 0; i < entities.size(); i++) {
#ifndef SERVER
		if (CGE::instance->thePlayer != entities[i])
#endif
			delete entities[i];
	}
	entities.clear();
	for (ChunksX::iterator x = chunkData.begin(); x != chunkData.end(); x++)
	{
		for (ChunksZ::iterator z = x->second.begin(); z != x->second.end(); z++)
		{
			for (ChunksY::iterator y = z->second.begin(); y != z->second.end(); y++)
			{
				delete y->second;
				y->second = 0;
			}
		}
	}
}

/**
 * \brief Превращает координаты пространства мира в координаты пространства чанков.
 * \param p Координаты для конвертации
 */
Pos World::convertWS2CS(Pos p)
{
	Pos n;
	n.x = p.x / 16;
	n.y = p.y / 16;
	n.z = p.z / 16;
	
	
	if (p.x < 0 && p.x % 16 != 0)
		n.x--;
	if (p.y < 0 && p.y % 16 != 0)
		n.y--;
	if (p.z < 0 && p.z % 16 != 0)
		n.z--;
	return n;
}

/**
* \brief Превращает координаты пространства мира в координаты пространства блоков чанка.
* \param p Координаты для конвертации
*/
Pos World::convertWS2BS(Pos p)
{
	Pos n;
	n.x = (p.x < 0 ? p.x + 1 : p.x) % 16;
	n.y = p.y % 16;
	n.z = (p.z < 0 ? p.z + 1 : p.z) % 16;
	if (p.x < 0)
		n.x = 15 + n.x;
	if (p.y < 0)
		n.y--;
	if (p.z < 0)
		n.z = 15 + n.z;
	return n;
}

/**
* \brief Превращает координаты пространства блоков чанка в координаты пространства мира.
* \param c Координаты чанка в пространстве чанков
* \param p Координаты для конвертации
*/
Pos World::convertBS2WS(Pos c, Pos p)
{
	Pos res;
	res.x = c.x * 16 + p.x;
	res.y = c.y * 16 + p.y;
	res.z = c.z * 16 + p.z;

	/*
	if (c.x < 0)
		res.x--;

	if (c.z < 0)
		res.z--;*/
	return res;
}


/**
* \brief Получает чанк по осям. Возвращает nullptr, если чанк не найден.
* \param p Координаты
* \return Чанк или nullptr
*/
Chunk* World::getChunk(Pos p)
{
	if (__cached_chunk && __cached_chunk->getPos() == p)
		return __cached_chunk;

	if (!chunkData.empty()) {
		ChunksX::iterator x = chunkData.find(p.x);
		if (x != chunkData.end())
		{
			ChunksZ::iterator z = x->second.find(p.z);
			if (z != x->second.end())
			{
				ChunksY::iterator y = z->second.find(p.y);
				if (y != z->second.end())
				{
					return y->second;
				}
			}
		}
	}
	Chunk* v = nullptr;
	return v;
}
/**
* \brief Получает чанк по осям. Генерирует или загружает чанк, если он не найден.
* \param p Координаты
* \return Существующий чанк или новосозданный
*/
Chunk* World::getChunkOrLoad(Pos p)
{
	if (__cached_chunk && __cached_chunk->getPos() == p)
		return __cached_chunk;
	static std::mutex mGCOL_sync;
	//std::unique_lock<std::recursive_mutex>(mt);
	ChunksX::iterator x = chunkData.find(p.x);
	ChunksY::iterator y;
	ChunksZ::iterator z;
	unsigned char f = 0;

	std::function<Chunk*()> kekCheck = [&]() -> Chunk*
	{
		f = 0;
		if (x != chunkData.end())
		{
			f++;
			z = x->second.find(p.z);
			if (z != x->second.end())
			{
				f++;
				y = z->second.find(p.y);
				if (y != z->second.end())
				{
					f++;
					if (y->second)
						return y->second;
				}
			}
		}
		return nullptr;
	};

	if (Chunk* k = kekCheck())
	{
		return k;
	}

	if (isRemote)
		return nullptr;

	std::unique_lock<std::mutex> lock(mGCOL_sync);
	if (Chunk* k = kekCheck())
	{
		return k;
	}
	Chunk* c;
	std::string s = std::string("world/region/r") + std::to_string(p.x) + "." + std::to_string(p.y) + "." + std::to_string(p.z) + ".cgr";
	if (File(s).isFile()) {
		c = new Chunk(this, p);
		try {
			c->load();
		} catch (std::exception& e)
		{
			WARN(std::string("Could not load ") + s + ": " + e.what() + " (CORRUPTED FILE?)");
			delete c;
			c = generator.generate(p);
		}
	}
	else {
		c = generator.generate(p);
	}
	switch (f)
	{
	case 0:
		chunkData[p.x][p.z][p.y] = c;
		break;
	case 1:
		x->second[p.z][p.y] = c;
		break;
	case 2:
		z->second[p.y] = c;
		break;
	case 3:
		y->second = c;
	}

	return c;
}


/**
 * \brief Возвращает тайл по указаной позиции.
 * \param pos Позиция
 * \return Тайл (nullable)
 */
TileEntity* World::getTileEntity(Pos pos)
{
	if (__cached_tile && __cached_tile->getPos() == pos) {
		return __cached_tile;
	}
	if (pos.y < 0)
	{
		return nullptr;
	}
	Pos p = convertWS2CS(pos);
	Chunk* chunk = getChunkOrLoad(p);
	if (chunk) {
		__cached_tile = chunk->getTileEntity(convertWS2BS(pos));
		return __cached_tile;
	}
	return nullptr;
}

void World::updateBlock(Pos p)
{
	getBlock(p)->update(p, this);
}

Block* World::getBlock(Pos pos)
{
	if (pos.y < 0)
	{
		return nullptr;
	}
	Pos p = convertWS2CS(pos);
	Chunk* chunk = getChunkOrLoad(p);
	if (chunk) {
		return chunk->getBlock(convertWS2BS(pos));
	}
	return nullptr;
}

void World::updateBlocks(const Pos& pos)
{
	updateBlock({ pos.x - 1, pos.y, pos.z });
	updateBlock({ pos.x + 1, pos.y, pos.z });
	updateBlock({ pos.x, pos.y, pos.z - 1 });
	updateBlock({ pos.x, pos.y, pos.z + 1 });
	updateBlock({ pos.x, pos.y - 1, pos.z });
	updateBlock({ pos.x, pos.y + 1, pos.z });
	updateBlock({ pos.x, pos.y, pos.z });
}

/**
 * \brief Устанавливает блок в указаные координаты
 * \param x Блок
 * \param pos Координаты для установки
 */
void World::setBlock(Block* x, Pos pos)
{
	if (pos.y < 0)
	{
		return;
	}
	Chunk* chunk = getChunkOrLoad(convertWS2CS(pos));
	if (!chunk)
		return;
	Pos cs = convertWS2BS(pos);
	bool changed = chunk->setBlock(x, cs);

	if (!isRemote)
		updateBlocks(pos);
#ifndef SERVER
	if (changed)
	{
		chunk->markAsDirty();

		if (cs.x == 0)
		{
			if (Chunk* c = chunk->getCLeft())
			{
				c->markAsDirty();
			}
		}
		if (cs.x == 15)
		{
			if (Chunk* c = chunk->getCRight())
			{
				c->markAsDirty();
			}
		}
		if (cs.z == 0)
		{
			if (Chunk* c = chunk->getCBack())
			{
				c->markAsDirty();
			}
		}
		if (cs.z == 15)
		{
			if (Chunk* c = chunk->getCFront())
			{
				c->markAsDirty();
			}
		}

		if (cs.y == 0)
		{
			if (Chunk* c = chunk->getCBottom())
			{
				c->markAsDirty();
			}
		}
		if (cs.y == 15)
		{
			if (Chunk* c = chunk->getCTop())
			{
				c->markAsDirty();
			}
		}
	}
#endif
}

/**
* \brief Устанавливает тайл в указаные координаты
* \param x Тайл
* \param pos Координаты для установки
*/
void World::setTileEntity(TileEntity* x, Pos pos)
{
	if (__cached_tile == x) {
		__cached_tile = x;
	}
	if (pos.y < 0)
	{
		return;
	}
	Chunk* chunk = getChunkOrLoad(convertWS2CS(pos));
	if (!chunk)
		return;
	Pos cs = convertWS2BS(pos);
	bool changed = chunk->setTileEntity(x, cs);
	x->setPos(pos);

	if (!isRemote)
		updateBlocks(pos);
#ifndef SERVER
	if (changed)
	{
		chunk->markAsDirty();

		if (cs.x == 0)
		{
			if (Chunk* c = chunk->getCLeft())
			{
				c->markAsDirty();
			}
		}
		if (cs.x == 15)
		{
			if (Chunk* c = chunk->getCRight())
			{
				c->markAsDirty();
			}
		}
		if (cs.z == 0)
		{
			if (Chunk* c = chunk->getCBack())
			{
				c->markAsDirty();
			}
		}
		if (cs.z == 15)
		{
			if (Chunk* c = chunk->getCFront())
			{
				c->markAsDirty();
			}
		}

		if (cs.y == 0)
		{
			if (Chunk* c = chunk->getCBottom())
			{
				c->markAsDirty();
			}
		}
		if (cs.y == 15)
		{
			if (Chunk* c = chunk->getCTop())
			{
				c->markAsDirty();
			}
		}
	}
#endif
}

void World::setBlockPython(Block*& x, Pos p)
{
	CGE::instance->server.raw()->runOnServerThread([&, x, p]()
	{
		setBlock(x, p);
	});
}


/**
 * \brief Тик мира
 */
void World::tick() {
	time += 1;
	time %= 48000;
	if (!isRemote) {
		for (size_t i = 0; i < entities.size(); i++)
		{
			for (size_t j = i + 1; j < entities.size(); j++)
			{
				Entity* e1 = entities[i];
				Entity* e2 = entities[j];
				AABB bb1 = e1->createAABB();
				AABB bb2 = e2->createAABB();
				if (bb1.test(bb2))
				{
					bool k1 = e1->onCollideWith(e2);
					bool k2 = e2->onCollideWith(e1);
					if (k1 && k2) {

						glm::vec3 f = bb2.getNearestPointTo(bb1.getNearestPointTo(bb2.getPos())) - bb1.getNearestPointTo(bb2.getNearestPointTo(bb1.getPos()));
						f = glm::normalize(f);
						if (f.x != f.x)
							f.x = 0.f;
						if (f.y != f.y)
							f.y = 0.f;
						if (f.z != f.z)
							f.z = 0.f;


						// TODO инертность тел

						glm::vec3 mot = e1->motion;
						glm::vec3 mot2 = e2->motion;
						if (EntityPlayerMP* e = dynamic_cast<EntityPlayerMP*>(e1))
						{
							mot = (e->getPos().toVec3() - e->getPrevPos().toVec3()) * 10.f;
						}
						if (EntityPlayerMP* e = dynamic_cast<EntityPlayerMP*>(e2))
						{
							mot2 = (e->getPos().toVec3() - e->getPrevPos().toVec3()) * 10.f;
						}
						float ff = (e2->weight) / (e1->weight + e2->weight);
						glm::vec3 af = glm::mix(mot, mot2, ff);
						e1->applyMotion((glm::max)((af - e1->motion), glm::vec3(1.f)) * -f);
						e2->applyMotion((glm::max)((af - e2->motion), glm::vec3(1.f)) * f);
					}
				}
			}
			entities[i]->tick();
		}
	}
}

Pos World::getHighestPoint(p_type x, p_type z)
{
	Pos p_prev(x, 0, z);
	for (size_t i = 0;; i++)
	{
		Pos p = Pos(x, 0, z);
		p = convertWS2CS(p);
		p.y = i;
		for (size_t y = 0; y < 16; ++y)
		{
			Chunk* chunk = getChunkOrLoad(p);
			Pos cur(p.x, p.y + y, p.z);
			Block* h = chunk->getBlock(cur);
			if (h->isAir())
			{
				return p_prev;
			}
			p_prev = convertBS2WS(p, cur);
		}
	}
}

/**
 * \brief Считает вектор солнца в соответствии с текущим временем.
 */
glm::vec3 World::calculateSunPos()
{
	int x = time + 4000;
	return glm::vec3(cosf(float(x) / glm::pi<float>() / 2400.f), -sinf(float(x) / glm::pi<float>() / 2400.f), (cosf(float(x) / glm::pi<float>() / 2400.f) + 1) / -2.f);
}

/**
 * \brief Проверка коллизии с миром.
 * \param bb Баундинг бокс
 * \param res Блок, с которым есть столкновение
 * \return true, если есть коллизия
 */
bool World::checkCollision(const AABB& bb, Pos& res)
{
	Pos mi(floor(bb.min.x), floor(bb.min.y), floor(bb.min.z));
	Pos ma(ceil(bb.max.x), ceil(bb.max.y), ceil(bb.max.z));

	for (p_type x = mi.x; x < ma.x; x++)
		for (p_type z = mi.z; z < ma.z; z++)
			for (p_type y = max(mi.y, 0); y < ma.y; y++)
			{
				Pos cur(x, y, z);
				Block* e = getBlock(cur);
				if (e && !e->isAir()) {
					res = cur;
					return true;
				}
			}
	return false;
}

void World::spawnEntity(Entity* e, DPos p)
{
	if (getEntity(e->getId()))
	{
		delete e;
		return;
	}
	e->worldObj = this;
	e->setPos(p);
	e->posRender = p;
	entities.push_back(e);

	if (!isRemote)
	{
		ts<IServer>::r(CGE::instance->server)->sendPacket(new PSpawnEntity(e));
		if (EntityPlayerMP* player = dynamic_cast<EntityPlayerMP*>(e))
		{
			player->selectChunks();
			player->getNetHandler()->sendPacket(new PContainerData(player, nullptr));
			ts<IServer>::r(CGE::instance->server)->sendPacketAllExcept(new PItemHeld(player, player->getHeldItem()), player->getNetHandler());
		}
	}
}

void World::unload(Pos p)
{
	Chunk* f = getChunk(p);
	if (__cached_chunk == f)
		__cached_chunk = nullptr;
	if (f) {
		if (__cached_tile && convertWS2CS(__cached_tile->getPos()) == f->getPos())
			__cached_tile = nullptr;
		delete f;
		chunkData[p.x][p.z][p.y] = nullptr;
#ifndef SERVER
		renderList.erase(std::find(renderList.begin(), renderList.end(), f));
#endif
	}
}

void World::removeEntity(Entity* entity)
{
	entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
	if (!isRemote)
	{
		ts<IServer>::r(CGE::instance->server)->sendPacket(new PDespawnEntity(entity));
	}
}

/**
 * \brief Загружает чанк в мир
 * \param chunk Чанк
 */
void World::loadChunk(Chunk* chunk)
{
	CGE::instance->uiThread.push([&, chunk]()
	{
		chunkData[chunk->getPos().x][chunk->getPos().z][chunk->getPos().y] = chunk;
#ifndef SERVER
		renderList.push_back(chunk);
#endif
	});
}

Entity* World::getEntity(size_t id)
{
	for (size_t i = 0; i < entities.size(); i++)
	{
		if (entities[i]->getId() == id)
		{
			return entities[i];
		}
	}
	return nullptr;
}

void World::setTime(world_time t)
{
	time = t;
	if (!isRemote)
	{
		ts<IServer>::r(CGE::instance->server)->sendPacket(new PWorldTime(time));
	}
}

void World::playSound(std::string name, const DPos& pos)
{
	if (!isRemote)
	{
		ts<IServer>::r(CGE::instance->server)->sendPacketAround(new PSound(name, pos), pos, 16.f);
	}
}
