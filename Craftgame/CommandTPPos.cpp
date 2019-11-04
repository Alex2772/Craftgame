#include "CommandTPPos.h"
#include "EntityPlayerMP.h"

void CommandTPPos::execute(CommandSender* c, std::vector<CmdLineArg>& args)
{
	if (PlayerCommandSender* p = dynamic_cast<PlayerCommandSender*>(c))
	{
		DPos pos;
		pos.x = args[0].toFloat();
		pos.y = args[1].toFloat();
		pos.z = args[2].toFloat();
		p->getPlayer()->setPos(pos);
	}
}
