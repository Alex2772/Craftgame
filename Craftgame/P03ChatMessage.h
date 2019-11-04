#pragma once
#include "Packet.h"

class P03ChatMessage: public Packet
{
private:
	std::string message;
public:
	P03ChatMessage(const std::string& message);
	P03ChatMessage();
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) override;
};
