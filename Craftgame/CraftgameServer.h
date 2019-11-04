#pragma once
#include "IServer.h"
#include <thread>
#include "Socket.h"
#include "NetPlayerHandler.h"
#include "AsyncQueue.h"

class CraftgameServer: public IServer
{
private:
	Socket serverSocket;
	typedef ts<std::map<Socket::Address, NetPlayerHandler*>> Clients;
	Clients clients;
	bool isRunning = true;
	std::thread connectionsThread;
	std::thread tickrateWatchdog;
	std::thread chunkSelect;
	typedef std::deque<NetPlayerHandler*> Players;
	Players players;
	uint8_t tick = 0;
	AsyncQueue<std::function<void()>> serverThreadFunctions;
	uint16_t ticks = 0;
public:
	uint16_t maxTickrate = 40;
	uint16_t currentTickrate = 0;
	std::vector<World*> worlds;
	CraftgameServer(const CraftgameServer&) = delete;
	CraftgameServer();
	~CraftgameServer();
	virtual void close() override;
	virtual void dropPlayer(const size_t id);
	virtual std::deque<NetPlayerHandler*>* getPlayers() { return &players; }
	virtual EntityPlayerMP* getPlayer(std::string name);
	virtual EntityPlayerMP* getPlayer(const size_t& id);
	virtual void runOnServerThread(std::function<void()> f);
	virtual void sendPacket(Packet* p); 
	virtual void dispatchCommand(CommandSender* sender, std::string& s);
	virtual void playerJoinWorld(EntityPlayerMP* e);
	virtual void sendPacketAllExcept(Packet*, NetPlayerHandler*);
	virtual void sendPacketAround(Packet* p, const DPos& pos, float radius);
	virtual void runServer();
	Socket& getSocket();
	void run();
	void removeClient(NetPlayerHandler* n);
};
