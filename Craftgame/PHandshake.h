#pragma once
#include "Packet.h"
#include "IServer.h"

class PHandshake: public Packet
{
public:
	PHandshake()
		: Packet(0x1b)
	{
	}
	server_info info;
	size_t id;
	boost::shared_ptr<ADT::Map> adt;
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
