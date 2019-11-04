#pragma once
#include "Command.h"

class CommandTickrate : public Command {
public:
	CommandTickrate() :
		Command("tickrate")
	{}
	virtual std::vector<Arg> getArgTypes() {
		return { Arg("tickrate", "", T_INTEGER, false) };
	}
	virtual std::string getHintsForArgs(CommandSender* c, std::vector<CmdLineArg>& args) {
		return "";
	}
	virtual void execute(CommandSender* c, std::vector<CmdLineArg>& args);
};