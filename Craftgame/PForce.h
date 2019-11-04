#pragma once

#include "Packet.h"

class PForce: public Packet
{
private:
	glm::vec3 force;
public:
	PForce(): Packet(0x1a)
	{
		
	}
	PForce(glm::vec3 f):
		PForce()
	{
		force = f;
	}
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};