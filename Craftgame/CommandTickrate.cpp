#include "CommandTickrate.h"
#include "IServer.h"
#include "GameEngine.h"
#include "CraftgameServer.h"

void CommandTickrate::execute(CommandSender* c, std::vector<CmdLineArg>& args)
{
	if (args.empty())
	{
		ts<IServer>::r server = CGE::instance->server;
		CraftgameServer* s = dynamic_cast<CraftgameServer*>(server.get());
		if (s)
			c->sendMessage(std::string("Current tickrate is ") + std::to_string(s->currentTickrate) + "/" + std::to_string(s->maxTickrate));
	} else
	{
		ts<IServer>::r server = CGE::instance->server;
		CraftgameServer* s = dynamic_cast<CraftgameServer*>(server.get());
		if (s)
		{
			s->maxTickrate = args[0].get<int>();
			c->sendMessage(std::string("Tickrate set to ") + std::to_string(s->maxTickrate));
		}
	}
}
