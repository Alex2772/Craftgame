#pragma once

#include "CommandSender.h"
#include "Parsing.h"
#include <memory.h>

class Arg {
public:
	Arg(std::string name, std::string n, ArgType type, bool need = true);
	std::string name;
	std::string n;
	ArgType type;
	bool need;
};

class Command {
private:
	std::string name;
public:
	Command(std::string _name):
	name(_name)
	{}
	virtual void execute(CommandSender* c, std::vector<CmdLineArg>& args) = 0;
	virtual std::vector<Arg> getArgTypes() = 0;
	virtual bool getAutocompletionForArgs(CommandSender* c, std::vector<CmdLineArg>& args) = 0;
	virtual std::string getHintsForArgs(CommandSender* c, std::vector<CmdLineArg>& args) = 0;
	std::string constructCommandUsage();
	std::string& getName();
};
