#include "PSpawnEntity.h"
#include "ByteBuffer.h"
#include "GameEngine.h"
#include <boost/smart_ptr/make_shared.hpp>

PSpawnEntity::PSpawnEntity(Entity* e) :
	Packet(0x08),
	adt(e->adtTag)
{
	tid = e->getNetId();
	id = e->getId();
	position = e->getPos();
	yaw = e->yawHead;
	pitch = e->pitch;
	e->pack();
}

PSpawnEntity::PSpawnEntity() :
	Packet(0x08)
{
}
void PSpawnEntity::write(ByteBuffer& buffer)
{
	buffer << tid << id << position << yaw << pitch;
	adt->serialize(buffer);
}

void PSpawnEntity::read(ByteBuffer& buffer)
{
	buffer >> tid >> id >> position >> yaw >> pitch;
	adt = boost::make_shared<ADT::Map>();
	adt->deserialize(buffer);
	adt->createIfNotExists();
}

void PSpawnEntity::onReceived()
{
#ifndef SERVER
	Entity* e = CGE::instance->entityRegistry->instanceEntity(tid, CGE::instance->thePlayer->worldObj, id);
	e->adtTag = adt;
	e->unpack();
	e->yawHead = yaw;
	e->pitch = pitch;
	CGE::instance->thePlayer->worldObj->spawnEntity(e, position);
#endif
}