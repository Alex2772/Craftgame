#pragma once
#include "Packet.h"


class PADTUpdate : public Packet {
public:
	boost::shared_ptr<ADT::Node> n;
	PADTUpdate():
		Packet(0xe)
	{}
	PADTUpdate(boost::shared_ptr<ADT::Node>);
	virtual void write(ByteBuffer& buffer);
	virtual void read(ByteBuffer& buffer);
	virtual void onReceived();
	virtual void onReceived(EntityPlayerMP*);
};
