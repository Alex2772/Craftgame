#include "Chunk.h"
#include "World.h"
#include "ChunkMesher.h"
#include "PacketStartLogin.h"
#include "GameEngine.h"
#include "Models.h"
#include "File.h"
#include "Dir.h"
#include "BinaryInputStream.h"
#include "BinaryOutputStream.h"
#include "Utils.h"
#include "BlockCustomRender.h"

ChunkPart::~ChunkPart()
{
	if (mesher)
		delete mesher;

#ifndef SERVER

	GLuint v = vao;

	GLuint* b = new GLuint[5];
	memcpy(b, buffers, sizeof(GLuint) * 5);

	CGE::instance->uiThread.push([v, b]()
	{
		if (v)
		{
			glDeleteVertexArrays(1, &v);
		}
		if (b[0])
		{
			glDeleteBuffers(5, b);
		}
		delete b;
	});
#endif
}

ChunkPartHandler::ChunkPartHandler()
{

}

ChunkPartHandler::~ChunkPartHandler()
{
	for (size_t i = 0; i < _infos.size(); i++)
	{
		delete _infos[i];
	}
}

bool ChunkPartHandler::isEmpty() const
{
	return _infos.empty();
}

ChunkPart* ChunkPartHandler::getCI(Material* m)
{
	for (size_t i = 0; i < _infos.size(); i++)
	{
		if (_infos[i]->material == m)
		{
			return _infos[i];
		}
	}
	ChunkPart* f = new ChunkPart;
	f->material = m;
	f->mesher = new ChunkMesher;
	_infos.push_back(f);
	return f;
}

int Chunk::CPos::toInt() const
{
	int res = x;
	res <<= 8;
	res |= y;
	res <<= 8;
	res |= z;
	return res;
}

Chunk::CPos::CPos():
x(0),
y(0),
z(0)
{

}

Chunk::CPos::CPos(int8_t x, int8_t y, int8_t z):
x(x),
y(y),
z(z)
{

}

bool Chunk::CPos::operator>(const CPos& b) const
{
	return toInt() > b.toInt();
}

bool Chunk::CPos::operator<(const CPos& b) const
{
	return toInt() < b.toInt();
}

/**
 * \brief Превращает блок в формат чанка (see Chunk::data)
 * \param b Блок
 * \return Блок в формате чанка
 */
uint32_t Chunk::toChunkFormat(Block* b)
{
	uint32_t result = b->getId();
	result <<= 8;
	result |= b->getData();
	return result;
}

/**
 * \brief Превращает блок (в формате чанка) в блок (see Chunk::data)
 * \param cf Блок в формате чанка
 * \return Блок
 */
Block* Chunk::fromChunkFormat(uint32_t cf)
{
	uint8_t data = uint8_t(cf & 0xff);
	uint32_t id = cf >> 8;
	return CGE::instance->goRegistry->getBlock(id, data);
}


Chunk::Chunk(
	World* w,
	Pos p):
	pos(p),
	worldObj(w)
{
	memset(data, 0, sizeof(data));
}

Chunk::~Chunk()
{
	for (auto& t : tileEntities)
		delete t.second;
	if (cLeft)
	{
		cLeft->cRight = nullptr;
	}
	if (cRight)
	{
		cRight->cLeft = nullptr;
	}
	if (cFront)
	{
		cFront->cBack = nullptr;
	}
	if (cBack)
	{
		cBack->cFront = nullptr;
	}
	if (cTop)
	{
		cTop->cBottom = nullptr;
	}
	if (cBottom)
	{
		cBottom->cTop = nullptr;
	}
#ifndef SERVER
	delete mesher;
#endif
}

/**
 * \brief Потокобезопасный метод получения тайла.
 * \param p Координаты чанкового пространства
 * \return Найденый тайл (Осторожно! @nullable!)
 */
TileEntity* Chunk::getTileEntity(Pos p)
{
	return tileEntities[CPos(p.x, p.y, p.z)];
}

/**
 * \brief Получение блока
 * \param p Координаты чанкового пространства
 * \return Блок
 */
Block* Chunk::getBlock(Pos p)
{
	return fromChunkFormat(data[p.x][p.y][p.z]);
}

/**
* \brief Устанавливает блок в координатах. Если блок требует TileEntity, будет вызвана World::setTileEntity
* \param block Блок
* \param p Координаты в чанковом пространтсве
* \return Возвращает true, если что-то изменилось и клиент об этом должен знать
*/
bool Chunk::setBlock(Block* block, Pos p)
{
	uint32_t to = toChunkFormat(block);
#ifndef SERVER
	Pos ws = World::convertBS2WS(pos, p);
	for (std::deque<TileEntity*>::iterator i = teCR.begin(); i != teCR.end(); ++i)
	{
		if ((*i)->getPos() == ws) {
			TileEntity* t = *i;
			teCR.erase(i);
			delete t;
			break;
		}
	}
#endif
	if (to != data[p.x][p.y][p.z])
	{
		if (TileEntity* t = block->newTileEntity())
		{
			setTileEntity(t, p);
		}
		else {
			data[p.x][p.y][p.z] = to;
			for (size_t i = 0; i < mChunkTrackers.size(); i++) {
				mChunkTrackers[i]->setBlock(block, World::convertBS2WS(pos, p));
			}
			markAsDirty();
		}
		return true;
	}
	return false;
}

/**
 * \brief Устанавливает блок в координатах
 * \param e Тайл
 * \param p Координаты в чанковом пространтсве
 * \return Возвращает true, если что-то изменилось и клиент об этом должен знать
 */
bool Chunk::setTileEntity(TileEntity* e, Pos p)
{
	bool changed = false;
	e->setPos(World::convertBS2WS(pos, p));
	if (tileEntities[CPos(p.x, p.y, p.z)])
		delete tileEntities[CPos(p.x, p.y, p.z)];
	tileEntities[CPos(p.x, p.y, p.z)] = e;
#ifndef SERVER
	Pos ws = World::convertBS2WS(pos, p);
	for (std::deque<TileEntity*>::iterator i = teCR.begin(); i != teCR.end(); ++i)
	{
		if ((*i)->getPos() == ws) {
			TileEntity* t = *i;
			teCR.erase(i);
			delete t;
			break;
		}
	}
	if (dynamic_cast<BlockCustomRender*>(e->getBlock()))
	{
		teCR.push_back(e);
	}
#endif
	for (size_t i = 0; i < mChunkTrackers.size(); i++) {
		mChunkTrackers[i]->setBlock(e->getBlock(), World::convertBS2WS(pos, p));
	}
	uint32_t t = toChunkFormat(e->getBlock());
	if (data[p.x][p.y][p.z] != t)
	{
		changed = true;
		data[p.x][p.y][p.z] = t;
	}
	modified |= changed;
	markAsDirty();
	return changed;
}


void Chunk::save() {
	if (!modified)
		return;
	Dir f("world");
	if (!f.isDir()) {
		f.mkdir();
	}
	f = Dir("world/region");
	if (!f.isDir()) {
		f.mkdir();
	}
	
	BinaryOutputStream fos(std::string("world/region/r") + std::to_string(pos.x) + "." + std::to_string(pos.y) + "." + std::to_string(pos.z) + ".cgr");

	if (fos.good()) {
		ByteBuffer buf;
		buf.reserve(0xffff);

		for (size_t i = 0; i < 16 * 16 * 16; i++) {
			Block* t = fromChunkFormat(data[i / 256][i / 16 % 16][i % 16]);
			buf.put(t->getId());
			byte b = t->getData();
			buf.put(b);
			TileEntity* te = tileEntities[CPos(i / 256, i / 16 % 16, i % 16)];
			ByteBuffer adt;
			if (te) {
				boost::shared_ptr<ADT::Map> m = te->serialize();
				if (m)
				{
					ADT::serialize(adt, m);
				}
			}
			buf << adt;
		}
		ByteBuffer c;
		buf.seekg(0);
		Utils::compress(buf, c);
		fos.write(c.get(), c.size());

		fos.close();
	}
}
void Chunk::load() {
	BinaryInputStream fis(std::string("world/region/r") + std::to_string(pos.x) + "." + std::to_string(pos.y) + "." + std::to_string(pos.z) + ".cgr");

	if (fis.good()) {
		ByteBuffer c;
		fis.seekg(0, std::ios::end);
		c.reserve(fis.tellg());
		c.setSize(fis.tellg());
		fis.seekg(0, std::ios::beg);
		fis.std::ifstream::read(c.get(), c.size());
		fis.close();

		ByteBuffer buf;
		Utils::decompress(c, buf);

		for (size_t i = 0; i < 16 * 16 * 16 && buf.count() < buf.size(); i++) {
			bid b;
			buf.get(b);
			byte data;
			buf.get(data);
			ByteBuffer a;
			buf >> a;
			Block* block = CGE::instance->goRegistry->getBlock(b, data);
			if (a.size()) {
				a.seekg(0);
				TileEntity* t = block->newTileEntity();
				boost::shared_ptr<ADT::Map> xc = ADT::deserialize(a)->toMap();
				if (t) {
					t->deserialize(xc);
					setTileEntity(t, Pos(i / 256, i / 16 % 16, i % 16));
				}
			}
			else
			{
				setBlock(block, Pos(i / 256, i / 16 % 16, i % 16));
			}
		}
	}
}

void Chunk::unload()
{
	if (!worldObj->isRemote)
		save();
	worldObj->unload(pos);
}

void Chunk::remeshAsync()
{
#ifndef SERVER
	state &= ~DIRTY;

	ts<IServer>::r(CGE::instance->server)->runAsync([&]()
	{
		remesh();
	});
#endif
}

/**
 * \brief Помечает меш чанка как "устаревший" и производит перестройку меша в отдельном потоке.
 */
void Chunk::markAsDirty()
{
#ifndef SERVER
	state |= DIRTY;
	if (isVisible()) {
		if (state & DIRTY && getCLeft() && getCRight() && getCTop() && (getCBottom() || pos.y == 0) && getCFront() && getCBack()) {
			remeshAsync();
		}
	}
#endif
}

bool Chunk::isVisible()
{
#ifdef SERVER
	return true;
#else
	return CGE::instance->renderer->frustum.isBoxInFrustum(createAABB());
#endif
}

/**
 * \brief Проверяет наличие полного непрозрачного блока по координатам пространства чанков.
 * Автоматически Обращается к соседним чанкам, если координаты указывают вне текущего.
 * 
 * АХТУНГ РЕКУРСИЯ
 * 
 * \param p Координаты
 * \return хз
 */
bool Chunk::blockCheck(Pos p)
{
	if (p.x < 0)
	{
		if (getCLeft()) {
			p.x += 16;
			return getCLeft()->blockCheck(p);
		}
		return false;
	}
	
	if (p.x > 15)
	{
		if (getCRight()) {
			p.x -= 16;
			return getCRight()->blockCheck(p);
		}
		return false;
	}

	if (p.y < 0)
	{
		if (getCBottom()) {
			p.y += 16;
			return getCBottom()->blockCheck(p);
		}
		return false;
	}
	if (p.y > 15)
	{
		if (getCTop()) {
			p.y -= 16;
			return getCTop()->blockCheck(p);
		}
		return false;
	}

	if (p.z < 0)
	{
		if (getCBack()) {
			p.z += 16;
			return getCBack()->blockCheck(p);
		}
		return false;
	}
	if (p.z > 15)
	{
		if (getCFront()) {
			p.z -= 16;
			return getCFront()->blockCheck(p);
		}
		return false;
	}
	Block* t = fromChunkFormat(data[p.x][p.y][p.z]);
	assert(t);
	return t->isSolid();
}

/**
 * \brief Пересоздаёт меш чанка для последующего залива в видеокарту функцией flush
 */
void Chunk::remesh()
{
#ifndef SERVER
	ChunkPartHandler* t = new ChunkPartHandler;
	for (unsigned char x = 0; x < 16; x++)
		for (unsigned char y = 0; y < 16; y++)
			for (unsigned char z = 0; z < 16; z++)
			{
				Block* block = fromChunkFormat(data[x][y][z]);
				if (block ->isSolid() && !block->isAir()) {
					BlockFaces bf;
					bf.left = blockCheck({ x - 1, y, z });
					bf.right = blockCheck({ x + 1, y, z });
					bf.top = blockCheck({ x, y + 1, z });
					bf.bottom = blockCheck({ x, y - 1, z });
					bf.front = blockCheck({ x, y, z + 1 });
					bf.back = blockCheck({ x, y, z - 1 });
					block->mesh(World::convertBS2WS(pos, {x, y, z}), t, bf);
				}
			}

	//CGE::instance->runOnSecondThread([&, t]() {flush(t); });
	CGE::instance->uiThread.push([&, t]() {flush(t); });
#endif
}
extern size_t _gDrawCalls;
/**
 * \brief Рендерит чанк.
 */
void Chunk::render(byte k)
{
	
#ifndef SERVER
	if ((state & DIRTY) && isVisible())
	{
		remeshAsync();
	}
	if (state & RENDER)
	{
		if (!mesher)
			return;
		for (std::vector<ChunkPart*>::iterator i = mesher->begin(); i != mesher->end(); ++i)
		{
			if ((*i)->icount) {
				if (k == 0)
					(*i)->material->bind();
				glBindVertexArray((*i)->vao);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*i)->buffers[4]);
				glDrawElements(GL_TRIANGLES, (*i)->icount, GL_UNSIGNED_INT, nullptr);
				++_gDrawCalls;
			}
		}
		for (std::deque<TileEntity*>::iterator i = teCR.begin(); i != teCR.end(); i++)
		{
			glm::vec3 c = DPos((*i)->getPos()).toVec3();
			c.x += 0.5f;
			c.z += 0.5f;
			if (BlockCustomRender* d = dynamic_cast<BlockCustomRender*>((*i)->getBlock()))
				d->renderBlock((*i)->getPos(), glm::translate(glm::mat4(1.0), c));
		}
		if (!teCR.empty())
		{
			CGE::instance->defaultStaticShader->loadMatrix("transform", glm::mat4(1.0));
			CGE::instance->renderer->setTexturingMode(1);
			CGE::instance->renderer->setColor(glm::vec4(1.f));
		}
	}
#endif
}

/**
 * \brief Обновляет меш чанка в видеокарте.
 */
void Chunk::flush(ChunkPartHandler* mesher)
{
#ifndef SERVER
	if (mesher) {
		for (std::vector<ChunkPart*>::iterator i = mesher->begin(); i != mesher->end(); i++)
		{
			if ((*i)->buffers[0]) {
				continue;
			}
			glGenBuffers(5, (*i)->buffers);
			glGenVertexArrays(1, &(*i)->vao);
			/*
			// Проверки на всякий случанский

			size_t count = cm->indices.size() / 6 * 4;

			if (!(count == cm->position.size() && count == cm->normal.size() && count == cm->tangent.size() && count == cm->uv.size()))
			{
			throw CraftgameException("WE  AR IN DEEP SHIT");
			}*/
			glBindVertexArray((*i)->vao);
			glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[0]);
			glBufferData(GL_ARRAY_BUFFER, (*i)->mesher->position.size() * sizeof(GLfloat) * 3, &((*i)->mesher->position[0].x), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[1]);
			glBufferData(GL_ARRAY_BUFFER, (*i)->mesher->uv.size() * sizeof(GLfloat) * 2, &((*i)->mesher->uv[0].x), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[2]);
			glBufferData(GL_ARRAY_BUFFER, (*i)->mesher->normal.size() * sizeof(GLfloat) * 3, &((*i)->mesher->normal[0].x), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[3]);
			glBufferData(GL_ARRAY_BUFFER, (*i)->mesher->tangent.size() * sizeof(GLfloat) * 3, &((*i)->mesher->tangent[0].x), GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*i)->buffers[4]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*i)->mesher->indices.size() * sizeof(GLuint), &((*i)->mesher->indices[0]), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glBindVertexArray(0);
			(*i)->icount = (*i)->mesher->indices.size();
			(*i)->mesher->position.clear();
			(*i)->mesher->uv.clear();
			(*i)->mesher->normal.clear();
			(*i)->mesher->tangent.clear();
			(*i)->mesher->indices.clear();
		}
		delete Chunk::mesher;
		Chunk::mesher = mesher;
		state = mesher->isEmpty() ? 0 : RENDER;
	}
#endif
}

/**
 * \brief Помечает окружающие чанки как устаревшие (Chunk::markAsDirty)
 */
void Chunk::remeshAround()
{
	if (Chunk* c = getCLeft())
	{
		c->markAsDirty();
	}
	if (Chunk* c = getCRight())
	{
		c->markAsDirty();
	}
	if (Chunk* c = getCFront())
	{
		c->markAsDirty();
	}
	if (Chunk* c = getCBack())
	{
		c->markAsDirty();
	}
	if (Chunk* c = getCTop())
	{
		c->markAsDirty();
	}
	if (Chunk* c = getCBottom())
	{
		c->markAsDirty();
	}
}

Chunk* Chunk::getCLeft()
{
	if (!cLeft)
	{
		cLeft = worldObj->getChunk(Pos(pos.x - 1, pos.y, pos.z));
		if (cLeft)
		{
			cLeft->cRight = this;
		}
	}
	return cLeft;
}
Chunk* Chunk::getCRight()
{
	if (!cRight)
	{
		cRight = worldObj->getChunk(Pos(pos.x + 1, pos.y, pos.z ));
		if (cRight)
		{
			cRight->cLeft = this;
		}
	}
	return cRight;
}
Chunk* Chunk::getCBack()
{
	if (!cBack)
	{
		cBack = worldObj->getChunk(Pos(pos.x, pos.y, pos.z - 1));
		if (cBack)
		{
			cBack->cFront = this;
		}
	}
	return cBack;
}
Chunk* Chunk::getCFront()
{
	if (!cFront)
	{
		cFront = worldObj->getChunk(Pos(pos.x, pos.y, pos.z + 1));
		if (cFront)
		{
			cFront->cBack = this;
		}
	}
	return cFront;
}
Chunk* Chunk::getCTop()
{
	if (!cTop)
	{
		cTop = worldObj->getChunk(Pos(pos.x, pos.y + 1, pos.z));
		if (cTop)
		{
			cTop->cBottom = this;
		}
	}
	return cTop;
}
Chunk* Chunk::getCBottom()
{
	if (!cBottom)
	{
		cBottom = worldObj->getChunk(Pos(pos.x, pos.y - 1, pos.z));
		if (cBottom)
		{
			cBottom->cTop = this;
		}
	}
	return cBottom;
}

AABB Chunk::createAABB()
{
	return { DPos(pos).toVec3() * glm::vec3(16.f), (DPos(pos).toVec3() + glm::vec3(1.f)) * glm::vec3(16.f) };
}

/**
 * \brief Очищает информацию о меше из видюхи и основной памяти (only ui thread)
 */
void Chunk::clear()
{
#ifndef SERVER
	delete mesher;
	state = UNKNOWN;
#endif
}
