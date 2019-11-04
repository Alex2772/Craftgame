#pragma once

#include "Command.h"

class CommandRegistry {
public:
	std::vector<Command*> commands;
	CommandRegistry();
	~CommandRegistry();
	void registerCommand(Command* cmd);
	std::vector<Command*>& getCommandList();
	Command* findCommandByName(std::string& c);
};