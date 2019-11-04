#pragma once

#include "EntityPlayerMP.h"
#include "Socket.h"
#include "ThreadPool.h"
#include <ctime>
#include "PlayerCommandSender.h"
#include "Packet.h"
#include "AsyncQueue.h"

class NetPlayerHandler {
private:
	bool isRunning = true;
	short ping = -1;
	ushort mps = 0; // chat Messages Per Seconds
	std::atomic<unsigned int> dllock = 0;
	Socket::Address address;
	bool handshakeDone = false;
	std::mutex stats_lck;
	std::atomic<uint16_t> s_rec = 0;
	std::atomic<uint16_t> s_snt = 0;
public:
	
	NetPlayerHandler(Socket::Address&);
	NetPlayerHandler(const NetPlayerHandler&) : commandSender(nullptr) {}
	~NetPlayerHandler();
	void disconnect(const std::string& cs);
	Socket::Address& getAddress();
	EntityPlayerMP* player = nullptr;
	EntityPlayerMP* getPlayer();
	PlayerCommandSender commandSender;
	std::chrono::milliseconds sendPingTime;
	std::time_t lastPacket = 0;
	void increaseMPS();
	void secTick();
	short getPing() const;
	void setPing(short);
	void sendPacket(std::shared_ptr<Packet>);
	void sendPacket(Packet*);
	void handlePacket(ByteBuffer& buf);
};
