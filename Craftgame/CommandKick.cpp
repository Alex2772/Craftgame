#include "CommandKick.h"
#include "GameEngine.h"

std::vector<std::string> CommandKick::getAutocompletionForArgs(CommandSender* c, std::vector<CmdLineArg>& args, size_t index)
{
	if (!args.empty() && index == 0 && args[0].type == T_STRING)
	{
		std::vector<std::string> r;
		ts<IServer>::r s(CGE::instance->server);
		std::deque<NetPlayerHandler*>* p = s->getPlayers();
		for (std::deque<NetPlayerHandler*>::iterator i = p->begin(); i != p->end(); i++) {
			std::string n = (*i)->getPlayer()->getGameProfile().getUsername();
			if (Parsing::startsWith(n, args[0].toString()))
				r.push_back(n);
		}
		return r;
	}
	return { args[index].toString() };
}

void CommandKick::execute(CommandSender * c, std::vector<CmdLineArg>& args)
{
	std::string reason = "­gui.disconnected.kick";
	if (args.size() > 1)
	{
		reason = "";
		for (size_t i = 1; i < args.size(); i++)
		{
			reason += args[i].data;
			if (i != args.size() - 1)
				reason += " "; // TODO
		}
	}
	ts<IServer>::rw s(CGE::instance->server);
	if (EntityPlayerMP* p = s->getPlayer(args[0].data))
	{
		p->getNetHandler()->disconnect(reason);
	}
}
