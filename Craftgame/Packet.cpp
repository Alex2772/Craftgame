#include "Packet.h"
#include "GameEngine.h"
#include "P06ShowUI.h"
#include "PSpawnEntity.h"
#include "PEntityMove.h"
#include "PDespawnEntity.h"
#include "PEntityAnimation.h"
#include "PFlyMode.h"
#include "POpenInventory.h"
#include "PContainerData.h"
#include "PContainerAction.h"
#include "PClick.h"
#include "PUpdateSlot.h"
#include "PChangeSlot.h"
#include "PCharging.h"
#include "P00Disconnected.h"
#include "P01Ping.h"
#include "P02JoinWorld.h"
#include "P03ChatMessage.h"
#include "P04ServerData.h"
#include "P05ChunkData.h"
#include "P06ShowUI.h"
#include "PWorldTime.h"
#include "P07SetBlock.h"
#include "PItemHeld.h"
#include "PEntityLook.h"
#include "PADTUpdate.h"
#include "PAutocomplete.h"
#include "PSound.h"
#include "PForce.h"
#include "PHandshake.h"

void Packets::reg()
{
	PacketRegistry* packetRegistry = CGE::instance->packetRegistry;

	packetRegistry->registerPacket<P00Disconnected>();
	packetRegistry->registerPacket<P01Ping>();
	packetRegistry->registerPacket<P02JoinWorld>();
	packetRegistry->registerPacket<P03ChatMessage>();
	packetRegistry->registerPacket<P04ServerData>();
	packetRegistry->registerPacket<P05ChunkData>();
	packetRegistry->registerPacket<P06ShowUI>();
	packetRegistry->registerPacket<P07SetBlock>();
	packetRegistry->registerPacket<PSpawnEntity>();
	packetRegistry->registerPacket<PEntityMove>();
	packetRegistry->registerPacket<PDespawnEntity>();
	packetRegistry->registerPacket<PEntityLook>();
	packetRegistry->registerPacket<PEntityAnimation>();
	packetRegistry->registerPacket<PWorldTime>();
	packetRegistry->registerPacket<PADTUpdate>();
	packetRegistry->registerPacket<PFlyMode>();
	packetRegistry->registerPacket<PAutocomplete>();
	packetRegistry->registerPacket<POpenInventory>();
	packetRegistry->registerPacket<PContainerData>();
	packetRegistry->registerPacket<PContainerAction>();
	packetRegistry->registerPacket<PClick>();
	packetRegistry->registerPacket<PUpdateSlot>();
	packetRegistry->registerPacket<PSound>();
	packetRegistry->registerPacket<PChangeSlot>();
	packetRegistry->registerPacket<PCharging>();
	packetRegistry->registerPacket<PItemHeld>();
	packetRegistry->registerPacket<PForce>();
	packetRegistry->registerPacket<PHandshake>();
}
