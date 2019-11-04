#pragma once
#include "Packet.h"
#include "ByteBuffer.h"

class P05ChunkData: public Packet
{
private:
	Chunk* chunk;
public:
	P05ChunkData() :
		Packet(5)
	{

	}
	P05ChunkData(Chunk* c) :
		Packet(5),
		chunk(c)
	{

	}
	virtual void write(ByteBuffer& buffer);
	virtual void read(ByteBuffer& buffer);
	virtual void onReceived(EntityPlayerMP* s) {
		
	}
	virtual void onReceived();
};
