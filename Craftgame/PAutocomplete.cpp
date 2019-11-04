#include "PAutocomplete.h"
#include "Command.h"
#ifndef SERVER
#include "GameEngine.h"
#include "GuiScreenChat.h"
#endif
void PAutocomplete::onReceived()
{
#ifndef SERVER
	std::vector<std::string> a = autocomplete;
	CGE::instance->uiThread.push([a]()
	{
		for (std::deque<boost::shared_ptr<GuiScreen>>::iterator i = CGE::instance->guiScreens.begin(); i != CGE::instance->guiScreens.end(); i++)
		{
			if (GuiScreenChat* c = dynamic_cast<GuiScreenChat*>(i->get()))
			{
				c->autocompleteReceived(a);
				break;
			}
		}
	});
#endif
}

void PAutocomplete::onReceived(EntityPlayerMP* p)
{
	if (mode == 0)
	{
		std::vector<CmdLineArg> args;
		Command* cmd = nullptr;
		Parsing::parseArgsServer(args, cmd, PAutocomplete::cmd.substr(1));
		if (cmd && args.size() > index)
		{
			PAutocomplete* a = new PAutocomplete();
			a->mode = 1;
			a->autocomplete = cmd->getAutocompletionForArgs(&p->getNetHandler()->commandSender, args, index);
			p->getNetHandler()->sendPacket(a);
		}
	}
}
