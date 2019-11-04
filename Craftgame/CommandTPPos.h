#pragma once

#include "Command.h"
#include "PlayerCommandSender.h"

class CommandTPPos : public Command {
public:
	CommandTPPos() :
		Command("tppos")
	{}
	virtual std::vector<Arg> getArgTypes() {
		return { Arg("x", "", T_NUMBER, true), Arg("y", "", T_NUMBER, true), Arg("z", "", T_NUMBER, true) };
	}
	virtual std::string getHintsForArgs(CommandSender* c, std::vector<CmdLineArg>& args) {
		return "";
	}

	void execute(CommandSender* c, std::vector<CmdLineArg>& args) override;
};
