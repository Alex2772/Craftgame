#pragma once

#include "Server.h"
#include "Socket.h"
#include <functional>
#include <ctime>
#include <queue>
#include <mutex>

class RemoteServer : public Server {
private:
	Socket* socket;
	std::queue<std::function<void()>> serverThread;
	bool isRunning = true;
public:
	std::time_t lastResponse;
	std::mutex serverThreadLock;
	RemoteServer(Socket* socket);
	~RemoteServer();
	virtual void sendPacket(Packet* packet);
	virtual Packet* readPacket();
	virtual void close();
	virtual void dispatchCommand(CommandSender* sender, std::string& command);

	virtual void runOnServerThread(std::function<void()> func);
	virtual void tick();
};