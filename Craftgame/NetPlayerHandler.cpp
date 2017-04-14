#include "NetPlayerHandler.h"

NetPlayerHandler::NetPlayerHandler(EntityPlayerMP* p, Socket* s):
	player(p),
	socket(s),
	t(1)
{
}