#pragma once
#include <string>

class CommandSender {
public:
	CommandSender() {}
	virtual void sendMessage(std::string message) = 0;
};