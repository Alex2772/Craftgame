#pragma once
#include "Packet.h"
#include "ByteBuffer.h"

class P04ServerData: public Packet
{
public:
	char type;
	id_type id;
	std::string name;
	short ping; // 2
	char state; // 3
	uint16_t p_rec = 0; // 4
	uint16_t p_snt = 0; // 4

	P04ServerData* setPing(short p)
	{
		ping = p;
		return this;
	}

	P04ServerData()
		: Packet(4)
	{
	}
	P04ServerData(EntityPlayerMP* mp)
		: P04ServerData()
	{
		type = 0; // CREATE
		if (mp) {
			id = mp->getId();
			name = mp->getGameProfile().getUsername();
			ping = mp->getNetHandler()->getPing();
		}
	}
	P04ServerData(char t, EntityPlayerMP* mp)
		: P04ServerData(mp)
	{
		type = t;
	}

	virtual void write(ByteBuffer& buffer) override {
		unsigned short tt = 0;
		buffer << type;
		buffer << id;
		switch (type)
		{
		case 0: // CREATE
			buffer << name << ping;
			break;
		case 2: { // UPDATE PING
			buffer << ping;
			break;
		}
		case 3: // UPDATE STATE
			buffer << state;
			break;
		case 4: // PACKET DATA
			buffer << p_rec << p_snt;
			break;
		}
	}
	virtual void read(ByteBuffer& buffer) override {
		buffer >> type >> id;
		switch (type)
		{
		case 0: // CREATE
			buffer >> name >> ping;
			break;
		case 2: // UPDATE PING
			buffer >> ping;
			break;
		case 3: // UPDATE PING
			buffer >> state;
			break;
		case 4: // PACKET DATA
			buffer >> p_rec >> p_snt;
			break;
		}
	}
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP* pl) override;
};

