#include "P05ChunkData.h"
#include "GameEngine.h"

void P05ChunkData::write(ByteBuffer& buffer)
{
	//TODO оптимизировать
	Pos p = chunk->getPos();
	buffer << p;
	buffer.reserve((sizeof(bid) + sizeof(byte)) * 4096);
	for (unsigned char x = 0; x < 16; x++)
		for (unsigned char y = 0; y < 16; y++)
			for (unsigned char z = 0; z < 16; z++)
			{
				Block* t = chunk->getBlock(Pos(x, y, z));
				bid i = t->getId();
				byte d = t->getData();
				buffer << i;
				buffer << d;
			}
}

void P05ChunkData::read(ByteBuffer& buffer)
{
#ifndef SERVER
	Pos p;
	buffer >> p;
	chunk = new Chunk(CGE::instance->thePlayer->worldObj, p);
	//ts<IServer>::r lock(CGE::instance->server);
	for (unsigned char x = 0; x < 16; x++)
		for (unsigned char y = 0; y < 16; y++)
			for (unsigned char z = 0; z < 16; z++)
			{
				bid id;
				byte d;
				buffer >> id;
				buffer >> d;
				chunk->setBlock(CGE::instance->goRegistry->getBlock(id, d), {x,y,z});
				//chunk->setTileEntity(new TileEntity(CGE::instance->goRegistry->getBlock(0, 0)), { x,y,z });
			}
#endif
}

void P05ChunkData::onReceived()
{
#ifndef SERVER
	if (CGE::instance->server) {
		CGE::instance->thePlayer->worldObj->loadChunk(chunk);
		chunk->markAsDirty();
		chunk->remeshAround();
	}
	else
	{
		delete chunk;
	}
#endif
}
