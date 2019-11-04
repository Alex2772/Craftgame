#include "CommandHelp.h"
#include "GameEngine.h"

void CommandHelp::execute(CommandSender* c, std::vector<CmdLineArg>& args)
{
	for (size_t i = 0; i < CGE::instance->commandRegistry->commands.size(); i++)
	{
		Command* cmd = CGE::instance->commandRegistry->commands[i];
		c->sendMessage(cmd->constructCommandUsage());
	}
}
