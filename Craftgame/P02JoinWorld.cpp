#include "P02JoinWorld.h"
#include "P04ServerData.h"
#include "GameEngine.h"

P02JoinWorld::P02JoinWorld():
	Packet(2)
{
}


void P02JoinWorld::write(ByteBuffer& buffer)
{
}

void P02JoinWorld::read(ByteBuffer& buffer)
{
}

void P02JoinWorld::onReceived()
{
}

void P02JoinWorld::onReceived(EntityPlayerMP* s)
{
	ts<IServer>::rw r(CGE::instance->server);
	if (!s->worldObj) {
		r->playerJoinWorld(s);
		// TODO поддержка модов
		r->sendPacket(new P04ServerData(s));
		for (std::deque<NetPlayerHandler*>::iterator i = r->getPlayers()->begin(); i != r->getPlayers()->end(); ++i)
		{
			NetPlayerHandler* net = *i;
			if (net->player != s)
			{
				P04ServerData* w = new P04ServerData(3, net->player);
				w->state = net->player->state;
				s->getNetHandler()->sendPacket(new P04ServerData(net->player));
				s->getNetHandler()->sendPacket(w);
			}
		}
	} else
	{
		s->getNetHandler()->disconnect(std::string("You are already joined world"));
	}
}
