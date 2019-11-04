#pragma once

#include "Command.h"

class CommandHelp : public Command {
public:
	CommandHelp() :
		Command("help")
	{}
	virtual std::vector<Arg> getArgTypes() {
		return{};
	}
	virtual std::string getHintsForArgs(CommandSender* c, std::vector<CmdLineArg>& args) {
		return "";
	}

	void execute(CommandSender* c, std::vector<CmdLineArg>& args) override;
};