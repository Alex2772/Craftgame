#include "PlayerCommandSender.h"
#include "P03ChatMessage.h"
#include "GameEngine.h"
#include "EntityPlayerMP.h"
#include <codecvt>

PlayerCommandSender::PlayerCommandSender(EntityPlayerMP* p):
	player(p)
{

}
void PlayerCommandSender::sendMessage(std::string s) {
	player->getNetHandler()->sendPacket(new P03ChatMessage(s));
}