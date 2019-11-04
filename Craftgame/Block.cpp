#include "Block.h"
#include "GameEngine.h"
#include "World.h"
#include "PClick.h"

Block::Block(_R _r, bid _id, byte _data):
	Item(_r, _id, _data, this)
{
#ifndef SERVER
	static Material* m = CGE::instance->materialRegistry->getMaterial("notexture");
	material = m;
#endif
}

Material * Block::getMaterialBySide(BlockSide side)
{
	return material;
}

/**
 * \brief явл€етс€ ли газом
 */
bool Block::isAir()
{
	return air;
}

/**
 * \brief явл€етс€ ли полноценным блоком
 */
bool Block::isSolid()
{
	return solid;
}

/**
 * \brief ќпредел€ет непрозрачность (пропуск света(
 */
bool Block::isOpaque()
{
	return opaque;
}

void Block::update(const Pos& e, World* w)
{
}

TileEntity* Block::newTileEntity()
{
	return nullptr;
}

void Block::render(Type t, Item* item, ItemStack* stack)
{
#ifndef SERVER
	if (t == FIRST_PERSON)
	{
		CGE::instance->renderer->transform = glm::translate(CGE::instance->renderer->transform, glm::vec3(0, -0.5f, 0));
	} else if (t == INVENTORY) {
		CGE::instance->renderer->transform = glm::rotate(CGE::instance->renderer->transform, float(glm::radians(90.f / M_PI)), glm::vec3(0.5, 0, 0));
		CGE::instance->renderer->transform = glm::rotate(CGE::instance->renderer->transform, glm::radians(45.f), glm::vec3(0, 0.5f, 0));
	}
	CGE::instance->renderer->renderBlock(this, CGE::instance->renderer->transform);
#endif
}

bool Block::onRightClick(const Pos& t, EntityPlayerMP* player)
{
	return true;
}

bool Block::breakBlock(const Pos& t, EntityPlayerMP* player)
{
	return true;
}

bool Block::placeBlock(const Pos& pos, EntityPlayerMP* player)
{
	return true;
}

bool Block::itemRightClick(EntityPlayerMP* sabj, ItemStack* stack)
{
	return true;
}

/**
* \brief ƒефолтный заполнитель меша.
* \param mesher „анк мешер
* \param bf ќкружение блоками
*/
void Block::mesh(const Pos& pos, ChunkPartHandler* mesher, const BlockFaces& bf)
{
	if (!bf.left)
	{
		CFace face;
		face.addVertex(glm::vec3(pos.x, pos.y + 1, pos.z + 1), glm::vec2(0, 0));
		face.addVertex(glm::vec3(pos.x, pos.y + 1, pos.z), glm::vec2(1, 0));
		face.addVertex(glm::vec3(pos.x, pos.y, pos.z + 1), glm::vec2(0, 1));
		face.addVertex(glm::vec3(pos.x, pos.y, pos.z), glm::vec2(1, 1));
		mesher->getCI(getMaterialBySide(Block::LEFT))->mesher->addFace(face);
	}
	if (!bf.right)
	{
		CFace face;
		face.addVertex(glm::vec3(pos.x + 1, pos.y + 1, pos.z), glm::vec2(0, 0));
		face.addVertex(glm::vec3(pos.x + 1, pos.y + 1, pos.z + 1), glm::vec2(1, 0));
		face.addVertex(glm::vec3(pos.x + 1, pos.y, pos.z), glm::vec2(0, 1));
		face.addVertex(glm::vec3(pos.x + 1, pos.y, pos.z + 1), glm::vec2(1, 1));
		mesher->getCI(getMaterialBySide(Block::RIGHT))->mesher->addFace(face);
	}
	if (!bf.back)
	{
		CFace face;
		face.addVertex(glm::vec3(pos.x, pos.y + 1, pos.z), glm::vec2(0, 0));
		face.addVertex(glm::vec3(pos.x + 1, pos.y + 1, pos.z), glm::vec2(1, 0));
		face.addVertex(glm::vec3(pos.x, pos.y, pos.z), glm::vec2(0, 1));
		face.addVertex(glm::vec3(pos.x + 1, pos.y, pos.z), glm::vec2(1, 1));
		mesher->getCI(getMaterialBySide(Block::BACK))->mesher->addFace(face);
	}
	if (!bf.front)
	{
		CFace face;
		face.addVertex(glm::vec3(pos.x + 1, pos.y + 1, pos.z + 1), glm::vec2(0, 0));
		face.addVertex(glm::vec3(pos.x, pos.y + 1, pos.z + 1), glm::vec2(1, 0));
		face.addVertex(glm::vec3(pos.x + 1, pos.y, pos.z + 1), glm::vec2(0, 1));
		face.addVertex(glm::vec3(pos.x, pos.y, pos.z + 1), glm::vec2(1, 1));
		mesher->getCI(getMaterialBySide(Block::FRONT))->mesher->addFace(face);
	}
	if (!bf.bottom)
	{
		CFace face;
		face.addVertex(glm::vec3(pos.x, pos.y, pos.z), glm::vec2(0, 0));
		face.addVertex(glm::vec3(pos.x + 1, pos.y, pos.z), glm::vec2(1, 0));
		face.addVertex(glm::vec3(pos.x, pos.y, pos.z + 1), glm::vec2(0, 1));
		face.addVertex(glm::vec3(pos.x + 1, pos.y, pos.z + 1), glm::vec2(1, 1));
		mesher->getCI(getMaterialBySide(Block::BOTTOM))->mesher->addFace(face);
	}
	if (!bf.top)
	{
		CFace face;
		face.addVertex(glm::vec3(pos.x, pos.y + 1, pos.z + 1), glm::vec2(0, 0));
		face.addVertex(glm::vec3(pos.x + 1, pos.y + 1, pos.z + 1), glm::vec2(1, 0));
		face.addVertex(glm::vec3(pos.x, pos.y + 1, pos.z), glm::vec2(0, 1));
		face.addVertex(glm::vec3(pos.x + 1, pos.y + 1, pos.z), glm::vec2(1, 1));
		mesher->getCI(getMaterialBySide(Block::TOP))->mesher->addFace(face);
	}
}
