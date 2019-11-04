#pragma once

#include "Command.h"

class CommandAlert : public Command {
public:
	CommandAlert() :
		Command("alert")
	{}
	virtual std::vector<Arg> getArgTypes() {
		return { Arg("message", "", T_STRING, false) };
	}
	virtual std::string getHintsForArgs(CommandSender* c, std::vector<CmdLineArg>& args) {
		return "";
	}

	void execute(CommandSender* c, std::vector<CmdLineArg>& args) override;
};