#pragma once

#pragma once
#include "Packet.h"
#include "ByteBuffer.h"

class PDespawnEntity : public Packet
{
private:
	id_type id;
public:
	PDespawnEntity(Entity*);
	PDespawnEntity();
	virtual void write(ByteBuffer& buffer) override {
		buffer << id;
	}
	virtual void read(ByteBuffer& buffer) override {
		buffer >> id;
	}
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) {};
};
