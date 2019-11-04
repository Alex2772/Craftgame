#pragma once

#include "Command.h"

class CommandStop : public Command {
public:
	CommandStop() :
		Command("stop")
	{}
	virtual std::vector<Arg> getArgTypes() {
		return{};
	}
	virtual std::string getHintsForArgs(CommandSender* c, std::vector<CmdLineArg>& args) {
		return "";
	}
	virtual void execute(CommandSender* c, std::vector<CmdLineArg>& args);
};