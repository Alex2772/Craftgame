#pragma once

#include "Command.h"

class CommandRegistry {
private:
	std::vector<Command*> commands;
public:
	CommandRegistry();
	~CommandRegistry();
	void registerCommand(Command* cmd);
	std::vector<Command*>* getCommandList();
	Command* findCommandByName(std::string& c);
};