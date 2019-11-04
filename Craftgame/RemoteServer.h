#pragma once
#include "IServer.h"
#include <string>
#include "Socket.h"
#include "World.h"
#include "AsyncQueue.h"
#include "NetServerHandler.h"
#include "GuiList.h"

class RemoteServer: public IServer
{
private:
	AsyncQueue<std::function<void()>> serverThreadFunctions;
	boost::thread* serverThread;
	bool isRunning = true;
public:
	NetServerHandler* net;
	Socket socket;
	RemoteServer(std::string url, unsigned short port);
	virtual ~RemoteServer();
	virtual void close();
	virtual void dropPlayer(const size_t) {}
	virtual void sendPacket(Packet* p);
	virtual void runOnServerThread(std::function<void()> f);

	virtual void sendPacketAllExcept(Packet*, NetPlayerHandler*) {}
	virtual std::vector<NetPlayerHandler*>* getNetHandler() { return nullptr; }
	void threadFunc();
	World* world;


#ifndef SERVER
	static std::shared_ptr<GuiList> playerList;
#endif
};
