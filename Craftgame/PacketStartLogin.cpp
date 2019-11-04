#include "PacketStartLogin.h"
#include "GameEngine.h"
#include "CraftgameServer.h"
#ifndef WINDOWS
#include <sys/socket.h>
#endif

#pragma warning(disable: 4996)
void PacketStartLogin::onReceived(EntityPlayerMP* p) {
	CraftgameServer* s = (CraftgameServer*)CGE::instance->server;

}
