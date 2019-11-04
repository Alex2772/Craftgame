#pragma once

#include "Command.h"

class CommandTime : public Command {
public:
	CommandTime() :
		Command("time")
	{}
	virtual std::vector<Arg> getArgTypes() {
		return {Arg("time", "", T_INTEGER, false)};
	}
	virtual std::string getHintsForArgs(CommandSender* c, std::vector<CmdLineArg>& args) {
		return "";
	}
	virtual void execute(CommandSender* c, std::vector<CmdLineArg>& args);
};