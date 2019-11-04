#include "P01Ping.h"
#include "ByteBuffer.h"
#include "GameEngine.h"
#include <ctime>

P01Ping::P01Ping() :
	Packet(1) {
	
}

void P01Ping::write(ByteBuffer& buffer) {
}
void P01Ping::read(ByteBuffer& buffer) {

}
void P01Ping::onReceived() {
	if (CGE::instance->server)
		ts<IServer>::r(CGE::instance->server)->sendPacket(new P01Ping);
}
void P01Ping::onReceived(EntityPlayerMP* p) {
	p->getNetHandler()->setPing(static_cast<short>((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()) - p->getNetHandler()->sendPingTime).count() / 2));
}
