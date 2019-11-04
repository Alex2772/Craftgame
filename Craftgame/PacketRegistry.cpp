#include "PacketRegistry.h"
#include "Packet.h"

PacketRegistry::PacketRegistry() {

}

PacketRegistry::~PacketRegistry() {
	for (size_t i = 0; i < packets.size(); i++)
		delete packets[i];
	packets.clear();
}

void PacketRegistry::registerPacket(InstancerBase* e) {
	packets.push_back(e);
}
/**
 * \brief Создаёт инстанцию пакета с ID p
 * \param p ID
 * \return Инстанция пакета
 */
Packet* PacketRegistry::instancePacket(unsigned short p) {
	if (p < packets.size())
	{
		return reinterpret_cast<Packet*>(packets[p]->create());
	}
	return nullptr;
}