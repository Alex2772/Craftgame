#pragma once

#include "Socket.h"
#include "Packet.h"
#include <thread>
#include "CommandSender.h"

class Server {
public:
	Server() {}
	virtual void sendPacket(Packet* packet) = 0;
	virtual Packet* readPacket() = 0;
	virtual void close() = 0;
	virtual void runOnServerThread(std::function<void()> func) = 0;
	virtual void tick() = 0;
	virtual void dispatchCommand(CommandSender* sender, std::string& command) = 0;
};