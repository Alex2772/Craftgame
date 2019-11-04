#pragma once
#include "global.h"
#include "Packet.h"
#include "CommandSender.h"
#include "PEntityMove.h"

struct server_info
{
	unsigned int verison = VERSION_CODE;
	bool scripts;
};
class IServer
{
private:
	ThreadPool tp;
public:
	server_info info;
	virtual ~IServer() = default;
	virtual void close() {}
	virtual void dropPlayer(const size_t get_id) {}
	virtual std::deque<NetPlayerHandler*>* getPlayers() { return nullptr; }
	virtual EntityPlayerMP* getPlayer(std::string name) { return nullptr; }
	virtual EntityPlayerMP* getPlayer(const size_t& id) { return nullptr; }
	virtual void runOnServerThread(std::function<void()>) {}
	virtual void sendPacket(Packet* p) {}
	virtual void sendPacketAround(Packet* p, const DPos& pos, float radius) {}
	virtual void sendPacketAllExcept(Packet* p, NetPlayerHandler* net_handler) {}

	virtual void dispatchCommand(CommandSender* sender, std::string& s) {}
	virtual void playerJoinWorld(EntityPlayerMP* e) {}
	virtual void runServer() {}
	virtual	void runAsync(std::function<void()> f)
	{
		tp.runAsync(f);
	};
};