#pragma once

#include "Command.h"

class CommandKick : public Command {
public:
	CommandKick() :
		Command("kick")
	{}
	virtual std::vector<Arg> getArgTypes() {
		return { Arg("player", "", T_STRING), Arg("reason", "", T_STRING, false) };
	}
	virtual std::vector<std::string> getAutocompletionForArgs(CommandSender* c, std::vector<CmdLineArg>& args, size_t index);
	virtual std::string getHintsForArgs(CommandSender* c, std::vector<CmdLineArg>& args) {
		return "";
	}
	virtual void execute(CommandSender* c, std::vector<CmdLineArg>& args);
};