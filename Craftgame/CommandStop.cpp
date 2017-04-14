#include "CommandStop.h"

#include "GameEngine.h"
#include "CraftgameServer.h"
void CommandStop::execute(CommandSender * c, std::vector<CmdLineArg>& args)
{
	c->sendMessage("Stopping the server...");
	CGE::instance->threadPool.runAsync([]() {
		((CraftgameServer*)CGE::instance->server)->initiateShutdown();
	});
}
