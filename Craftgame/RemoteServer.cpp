#include "RemoteServer.h"
#include "P00Disconnected.h"
#include <boost/thread/thread.hpp>

#ifndef SERVER
std::shared_ptr<GuiList> RemoteServer::playerList = std::make_shared<GuiList>();
#endif
RemoteServer::RemoteServer(std::string url, unsigned short port):
	socket(url, port),
	world(new World)
{
#ifndef SERVER
	serverThread = new boost::thread(boost::bind(&RemoteServer::threadFunc, this));
#endif
}

RemoteServer::~RemoteServer()
{
#ifndef SERVER
	if (isRunning)
		close();
	delete net;
	delete world;
	delete serverThread;
	CGE::instance->uiThread.push([&]()
	{
		playerList->clear();
	});
#endif
}

void RemoteServer::close()
{
	sendPacket(new P00Disconnected());
	isRunning = false;
	socket.close();
	serverThread->interrupt();
	serverThread->join();
}

void RemoteServer::runOnServerThread(std::function<void()> f)
{
	serverThreadFunctions.push(f);
}

void RemoteServer::sendPacket(Packet* p)
{
#ifndef SERVER
	net->sendPacket(p);
#endif
}
