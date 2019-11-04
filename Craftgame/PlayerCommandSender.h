#pragma once
#include "CommandSender.h"

class EntityPlayerMP;

class PlayerCommandSender : public CommandSender {
private:
	EntityPlayerMP* player;
public:
	PlayerCommandSender(EntityPlayerMP* p);
	virtual void sendMessage(std::string message);
	EntityPlayerMP* getPlayer()
	{
		return player;
	}
};