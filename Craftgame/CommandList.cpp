#include "CommandList.h"
#include "GameEngine.h"
#include "CraftgameServer.h"

void CommandList::execute(CommandSender* c, std::vector<CmdLineArg>& args) {
	std::vector<NetPlayerHandler> p = ((CraftgameServer*)CGE::instance->server)->players;
	c->sendMessage(std::string("There are ") + std::to_string(p.size()) + " player(s) on this server" + (p.size() ? ":" : "."));
	for (size_t i = 0; i < args.size(); i++)
		c->sendMessage(std::string(args[i].name) + " > " + std::string(args[i].data));
	for (size_t i = 0; i < p.size(); i++) {
		c->sendMessage(p[i].player->getGameProfile().getUsername());
	}
}