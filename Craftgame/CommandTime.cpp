#include "CommandTime.h"
#include "ThreadUtils.h"
#include "IServer.h"
#include "GameEngine.h"
#include "CraftgameServer.h"

void CommandTime::execute(CommandSender* c, std::vector<CmdLineArg>& args)
{
	if (args.size() == 1)
	{

		ts<IServer>::r s(CGE::instance->server);
		if (CraftgameServer* cgs = dynamic_cast<CraftgameServer*>(s.get())) {
			world_time t = args[0].get<int>() % 48000;
			cgs->worlds[0]->setTime(t);
			int hours = int(floorf(float(t) / 2000.f));
			int mins = int(float(t % 2000) / 2000.f * 60.f);
			std::string ms = std::to_string(mins);
			if (ms.length() == 1)
				ms = "0" + ms;
			c->sendMessage(std::string("Time set to ") + std::to_string(hours) + ":" + ms);
		}
	} else
	{
		ts<IServer>::r s(CGE::instance->server);
		if (CraftgameServer* cgs = dynamic_cast<CraftgameServer*>(s.get())) {
			world_time t = cgs->worlds[0]->time;
			int hours = int(floorf(float(t) / 2000.f));
			int mins = int(float(t % 2000) / 2000.f * 60.f);
			std::string ms = std::to_string(mins);
			if (ms.length() == 1)
				ms = "0" + ms;
			c->sendMessage(std::string("Current time is ") + std::to_string(hours) + ":" + ms);
		}

	}
}
