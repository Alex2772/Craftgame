#include "CommandAlert.h"
#include "GameEngine.h"
#include "P06ShowUI.h"

void CommandAlert::execute(CommandSender* c, std::vector<CmdLineArg>& args)
{
	std::string reason;
	if (!args.empty())
	{
		reason = "";
		for (size_t i = 0; i < args.size(); i++)
		{
			reason += args[i].data;
			if (i != args.size() - 1)
				reason += " "; // TODO
		}
	}
	ts<IServer>::rw(CGE::instance->server)->sendPacket(new P06ShowUI('d', reason));
}
