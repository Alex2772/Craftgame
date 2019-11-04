#include "P03ChatMessage.h"
#include "GameEngine.h"
#include "ByteBuffer.h"

P03ChatMessage::P03ChatMessage():
	Packet(0x03)
{
}
P03ChatMessage::P03ChatMessage(const std::string& message)
	: Packet(0x03), message(message)
{
}


void P03ChatMessage::write(ByteBuffer& buffer)
{
	buffer << message;
}

void P03ChatMessage::read(ByteBuffer& buffer)
{
	buffer >> message;
}

void P03ChatMessage::onReceived()
{
	std::string msg = message;
	CGE::instance->uiThread.push([msg]()
	{
		CGE::instance->chat->addChat(msg);
	});
}

void P03ChatMessage::onReceived(EntityPlayerMP* s)
{
	CGE::instance->chat->onPlayerChatMessage(s, message);
}
