#ifndef SERVER
#include "RemoteServer.h"
#include "ByteBuffer.h"
#include "GameEngine.h"

RemoteServer::RemoteServer(Socket* _s) :
	socket(_s) {
	lastResponse = std::time(0);
}
void RemoteServer::close() {
	if (socket) {
		socket->close();
		delete socket;
		socket = nullptr;
	}
}
RemoteServer::~RemoteServer() {
	close();
}

void RemoteServer::sendPacket(Packet* packet) {
	CGE::instance->threadPool.runAsync([&, packet]() {
		try {
			socket->write(packet->getPacketID());
			ByteBuffer bb;
			packet->write(bb);
			socket->write(bb.size());
			socket->writeBytes(bb.get(), bb.size());
		}
		catch ( CraftgameException e) {
			CGE::instance->logger->err(std::string("An error has occurred while sending packet to server: ") + e.what());
		}
		delete packet;
	});
}
Packet* RemoteServer::readPacket() {
	CGE::instance->threadPool.runAsync([&]() {
		unsigned short packet = socket->read<unsigned short>();
		Packet* p = CGE::instance->packetRegistry->getPacket(packet)->createInstance();
		ByteBuffer bb;
		size_t size = socket->read<size_t>();
		char* b = new char[size];
		socket->readBytes(b, size, 0);
		bb.put(b, size);
		delete[] b;
		p->read(bb);
		p->onReceived();
		delete p;
	});
	return nullptr;
}
void RemoteServer::tick() {
	if (isRunning) {
		std::unique_lock<std::mutex> lck(serverThreadLock);
		if (lastResponse + 10 < std::time(0)) {
			isRunning = false;
			CGE::instance->disconnect(nullptr, "Connection timeout", "Connection lost");
		} else {
			if (!serverThread.empty()) {
				serverThread.front()();
				serverThread.pop();
			}
		}
	}
}
void RemoteServer::runOnServerThread(std::function<void()> func) {
	std::unique_lock<std::mutex> lck(serverThreadLock);
	serverThread.push(func);
}

void RemoteServer::dispatchCommand(CommandSender* sender, std::string& command) {

}
#endif