#pragma once

#pragma once
#include "Packet.h"
#include "def.h"
#include "ADT.h"

class PSpawnEntity : public Packet
{
private:
	std::string name;
	id_type tid;
	id_type id;
	float yaw, pitch;
	DPos position;
	boost::shared_ptr<ADT::Map> adt;
public:
	PSpawnEntity(Entity*);
	PSpawnEntity();
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) {};
};
