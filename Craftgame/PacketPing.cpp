#include "PacketPing.h"
#include "ByteBuffer.h"
#include "GameEngine.h"
#include "RemoteServer.h"
#include <ctime>

Packet* PacketPing::createInstance() {
	return new PacketPing;
}

void PacketPing::write(ByteBuffer& buffer) {
	char a = 0;
	buffer << a;
}
void PacketPing::read(ByteBuffer& buffer) {

}
void PacketPing::onReceived() {
	((RemoteServer*)CGE::instance->server)->lastResponse = std::time(0);
}
void PacketPing::onReceived(EntityPlayerMP* p) {
	CGE::instance->server->sendPacket(new PacketPing);
}