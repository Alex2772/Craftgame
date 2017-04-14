#include "PacketRegistry.h"
#include "Packet.h"

PacketRegistry::PacketRegistry() {

}

PacketRegistry::~PacketRegistry() {
	for (size_t i = 0; i < packets.size(); i++)
		delete packets[i];
	packets.clear();
}

void PacketRegistry::registerPacket(Packet* p) {
	packets.push_back(p);
}
Packet* PacketRegistry::getPacket(unsigned short p) {
	for (size_t i = 0; i < packets.size(); i++) {
		if (packets[i]->getPacketID() == p)
			return packets[i];
	}
	return nullptr;
}