#pragma once
#include "Server.h"
#include "Socket.h"
#include "World.h"
#include "AsyncVector.h"
#include "AsyncQueue.h"
#include "EntityPlayerMP.h"
#include <functional>
#include "NetPlayerHandler.h"
#include "ConsoleCommandSender.h"

struct ID_sck {
	size_t id;
	Socket* socket;
	time_t joinTime;
};

class CraftgameServer : public Server {
private:
	ServerSocket* serverSocket = nullptr;
	std::thread* connectThread = nullptr;
	std::thread* connectionsWatcher = nullptr;
	AsyncQueue < std::function<void()>> serverThreadF;
public:
	ConsoleCommandSender* ccs;
	bool isRunning = true;
	std::vector<World*> worlds;
	AsyncVector<ID_sck> sockets;
	AsyncVector<NetPlayerHandler> players;
	CraftgameServer();
	~CraftgameServer();
	virtual void sendPacket(Packet* packet);
	virtual void sendPacketTo(Packet* packet, EntityPlayerMP* target);
	virtual void sendPacketTo(Packet* packet, Socket* target);
	virtual Packet* readPacket();
	virtual void close();
	virtual void runOnServerThread(std::function<void()> func);
	virtual void dispatchCommand(CommandSender* sender, std::string& command);
	virtual void tick();
	void disconnectPlayer(size_t entityId, std::string reason);
	void initiateShutdown();
	void run();
};