#include "CommandRegistry.h"

CommandRegistry::CommandRegistry() {

}
CommandRegistry::~CommandRegistry() {
	for (size_t i = 0; i < commands.size(); i++) {
		delete commands[i];
	}
	commands.clear();
}
void CommandRegistry::registerCommand(Command* cmd) {
	commands.push_back(cmd);
}
std::vector<Command*>& CommandRegistry::getCommandList()
{
	return commands;
}
Command* CommandRegistry::findCommandByName(std::string& c) {
	for (size_t i = 0; i < commands.size(); i++) {
		if (commands[i]->getName() == c) {
			return commands[i];
		}
	}
	return nullptr;
}