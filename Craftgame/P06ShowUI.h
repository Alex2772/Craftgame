#pragma once
#include "Packet.h"

class P06ShowUI : public Packet
{
private:
	char ui;
	std::string message;
public:
	P06ShowUI(char, const std::string&);
	P06ShowUI();
	virtual void write(ByteBuffer& buffer) override;
	virtual void read(ByteBuffer& buffer) override;
	virtual void onReceived() override;
	virtual void onReceived(EntityPlayerMP*) {};
};
