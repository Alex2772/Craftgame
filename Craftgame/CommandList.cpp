#include "CommandList.h"
#include "GameEngine.h"

void CommandList::execute(CommandSender* c, std::vector<CmdLineArg>& args) {
	ts<IServer>::r l(CGE::instance->server);
	std::deque<NetPlayerHandler*>* p = l->getPlayers();
	c->sendMessage(std::string("There are ") + std::to_string(p->size()) + " player(s) on this server" + (p->size() ? ":" : "."));
	for (size_t i = 0; i < args.size(); i++)
		c->sendMessage(std::string(args[i].name) + " > " + std::string(args[i].data));
	for (std::deque<NetPlayerHandler*>::iterator i = p->begin(); i != p->end(); i++) {
		c->sendMessage((*i)->player->getGameProfile().getUsername());
	}
}