#pragma once
#include "Packet.h"
#include "CSInterface.h"

class PContainerData: public Packet
{
private:
	CSInterface* cs;
	EntityPlayerMP* player = nullptr;
	ByteBuffer buffer;
public:
	PContainerData() :
		Packet(0x12)
	{
		
	}
	PContainerData(EntityPlayerMP* p, CSInterface* c) :
		PContainerData()
	{
		cs = c;
		player = p;
	}

	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
