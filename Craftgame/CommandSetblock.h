#pragma once
#include "Parsing.h"
#include "Command.h"

class CommandSetblock : public Command {
public:
	CommandSetblock() :
		Command("setblock")
	{}
	virtual std::vector<Arg> getArgTypes() {
		return { Arg("x", "", T_INTEGER), Arg("y", "", T_INTEGER), Arg("z", "", T_INTEGER), Arg("block", "", T_STRING) };
	}
	virtual std::vector<std::string> getAutocompletionForArgs(CommandSender* c, std::vector<CmdLineArg>& args, size_t index);
	virtual std::string getHintsForArgs(CommandSender* c, std::vector<CmdLineArg>& args) {
		return "";
	}
	virtual void execute(CommandSender* c, std::vector<CmdLineArg>& args);
};
