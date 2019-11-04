#pragma once
#include "CommandSender.h"

class ConsoleCommandSender : public CommandSender {
public:
	ConsoleCommandSender();
	virtual void sendMessage(std::string message);
};