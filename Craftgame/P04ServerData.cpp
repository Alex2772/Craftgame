#include "P04ServerData.h"
#include "GameEngine.h"
#include "RemoteServer.h"
#include "PlayerListEntry.h"

void P04ServerData::onReceived()
{
#ifndef SERVER
	char type = P04ServerData::type;
	id_type id = P04ServerData::id;
	std::string name = P04ServerData::name;
	short ping = P04ServerData::ping;
	char state = P04ServerData::state;
	uint16_t p_rec = P04ServerData::p_rec;
	uint16_t p_snt = P04ServerData::p_snt;
	CGE::instance->uiThread.push([type, id, name, ping, state, p_rec, p_snt] ()
	{
		switch (type)
		{
		case 0: // CREATE
			RemoteServer::playerList->addWidget(new PlayerListEntry(id, name, ping));
			break;
		case 1: // REMOVE
		{
			GuiContainer* f = RemoteServer::playerList.get();
			for (GuiContainer::Widgets::iterator i = f->widgets.begin(); i != f->widgets.end(); ++i)
			{
				if (dynamic_cast<PlayerListEntry*>((*i).get())->id == id)
				{
					f->widgets.erase(i);
					return;
				}
			}
			break;
		}
		case 2: // UPDATE PING
		{
			GuiContainer* f = RemoteServer::playerList.get();
			for (GuiContainer::Widgets::iterator i = f->widgets.begin(); i != f->widgets.end(); ++i)
			{
				PlayerListEntry* item = dynamic_cast<PlayerListEntry*>((*i).get());
				if (item->id == id)
				{
					item->ping = ping;
				}
			}
			break;
		}
		case 3: // UPDATE STATE
		{
			GuiContainer* f = RemoteServer::playerList.get();
			for (GuiContainer::Widgets::iterator i = f->widgets.begin(); i != f->widgets.end(); ++i)
			{
				PlayerListEntry* item = dynamic_cast<PlayerListEntry*>((*i).get());
				if (item->id == id)
				{
					item->state = state;
				}
			}
			break;
		}
		case 4: // PACKET DATA
		{
			ts<IServer>::r lock = CGE::instance->server;
			if (CGE::instance->server) {
				NetServerHandler* r = dynamic_cast<RemoteServer*>(lock.get())->net;
				r->s_rec += size_t(p_rec);
				r->s_snt += size_t(p_snt);
				r->p_loss_in = (r->s_snt >= r->c_rec) ? r->s_snt - r->c_rec : 0;
				r->p_loss_out = (r->c_snt >= r->s_rec) ? r->c_snt - r->s_rec : 0;
				break;
			}
		}
		}
	});
#endif
}

void P04ServerData::onReceived(EntityPlayerMP* pl)
{
	pl->state = state;
	P04ServerData* p = new P04ServerData(type, pl);
	p->ping = ping;
	p->state = state;
	ts<IServer>::r(CGE::instance->server)->sendPacket(p);
}
