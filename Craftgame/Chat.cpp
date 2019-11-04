#include "Chat.h"
#include "GameEngine.h"
#include "P03ChatMessage.h"
#include "EntityMutant.h"

Chat::Chat()
{
}

void Chat::broadcast(std::string s)
{
	INFO(s);
	ts<IServer>::r(CGE::instance->server)->sendPacket(new P03ChatMessage(s));
}

void Chat::onPlayerChatMessage(EntityPlayerMP* from, std::string message) {
	if (Parsing::startsWith(message, "/")) {
		INFO(from->getGameProfile().getUsername() + " issued server command: " + message);
		ts<IServer>::r(CGE::instance->server)->dispatchCommand(&from->getNetHandler()->commandSender, message.substr(1));
	}
	else {
		EntityMutant* t = new EntityMutant();
		t->setId(CGE::instance->assignGlobalUniqueEntityId());
		from->worldObj->spawnEntity(t, from->getPos());
		std::string m;
		if (from->getGameProfile().getUsername() == "Alex2772" || from->getGameProfile().getUsername() == "Gpn" || from->getGameProfile().getUsername() == "Dan7604") {
			m = std::string("§8[§4Craftgame§8]§6 ") + from->getGameProfile().getUsername() + "§8:§b " + message;
		}
		else {
			m = std::string("<") + from->getGameProfile().getUsername() + "> " + message;
		}
		CGE_EVENT("chat", CGE_P("player", from)CGE_P("message", message)CGE_P("format", m), [&]()
		{
			broadcast(m);
			from->getNetHandler()->increaseMPS();
		});
	}
}

void Chat::addChat(std::string m)
{
	chatQueue.push_front({ m, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() });
	while (chatQueue.size() > 50)
	{
		chatQueue.pop_back();
	}
}

void Chat::clear()
{
	CGE::instance->uiThread.push([&]()
	{
		chatQueue.clear();
	});
}

void Chat::sendMessage(std::string message)
{
	INFO(message);
	ts<IServer>::r(CGE::instance->server)->sendPacket(new P03ChatMessage(message));
}
