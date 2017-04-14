#include "PacketStartLogin.h"
#include "GameEngine.h"
#include "CraftgameServer.h"
#ifndef WINDOWS
#include <sys/socket.h>
#endif

#pragma warning(disable: 4996)
void PacketStartLogin::onReceived(EntityPlayerMP* p) {
	CraftgameServer* s = (CraftgameServer*)CGE::instance->server;
	for (size_t i = 0; i < s->sockets.size();) {
		ID_sck d = s->sockets[i];
		if (d.id == id) {
			s->sockets.erase(s->sockets.begin() + i);
			in_addr ia = (d.socket->server.sin_addr);
			INFO(std::string("Attempting connection from ") + inet_ntoa(ia));
			CraftgameServer* s = (CraftgameServer*)CGE::instance->server;
			EntityPlayerMP* player = new EntityPlayerMP(s->worlds[0], d.id, GameProfile(PacketStartLogin::login), d.socket);
			NetPlayerHandler npl(player, d.socket);
			s->players.push_back(npl);
			s->worlds[0]->entities.push_back(player);

			npl.t.runAsync([s, d, player]() {
				try {
					while (1) {
						Socket* s = d.socket;
						unsigned short packet = s->read<unsigned short>();
						Packet* p = CGE::instance->packetRegistry->getPacket(packet);
						ByteBuffer bb;
						size_t size = s->read<size_t>();
						char* b = new char[size];
						s->readBytes(b, size, 0);
						bb.put(b, size);
						delete[] b;
						p->read(bb);
						p->onReceived(player);
					}
				}
				catch ( CraftgameException e) {
					try {
						((CraftgameServer*)CGE::instance->server)->disconnectPlayer(d.id, "An exception has occurred on connection thread; this connection is terminated");
					}
					catch (...) {}
				}
			});
			INFO(PacketStartLogin::login + " joined the game");
		}
		else {
			i++;
		}
	}
}
