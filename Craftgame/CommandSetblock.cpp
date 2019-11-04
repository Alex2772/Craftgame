#include "CommandSetblock.h"
#include "GameEngine.h"

std::vector<std::string> CommandSetblock::getAutocompletionForArgs(CommandSender* c, std::vector<CmdLineArg>& args, size_t index)
{
	std::vector<std::string> r;
	if (args.size() == 4) {
		for (GORegistry::bdata::iterator i = CGE::instance->goRegistry->data.begin(); i != CGE::instance->goRegistry->data.end(); ++i)
		{
			if (dynamic_cast<Block*>(i->second)) {
				std::string n = i->first.full;
				if (Parsing::startsWith(n, args[3].toString()))
					r.push_back(n);
				else if (i->first.domain == "craftgame") {
					n = i->first.path;
					if (Parsing::startsWith(n, args[3].toString()))
						r.push_back(n);
				}
			}
		}
	}
	return r;
}

void CommandSetblock::execute(CommandSender * c, std::vector<CmdLineArg>& args)
{
	if (PlayerCommandSender* player = dynamic_cast<PlayerCommandSender*>(c))
	{
		Block* block = CGE::instance->goRegistry->getBlock(_R(args[3].data));
		if (block)
		{
			player->getPlayer()->worldObj->setBlock(block, Pos(args[0].get<int>(), args[1].get<int>(), args[2].get<int>()));
			player->sendMessage("Block set");
		}
		else {
			player->sendMessage("No such block");
		}
	}
}
