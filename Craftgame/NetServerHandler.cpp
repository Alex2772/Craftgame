#include "NetServerHandler.h"
#include "RemoteServer.h"
#include "GameEngine.h"
#include "ByteBuffer.h"
#include "Utils.h"
#include <boost/thread.hpp>

#ifndef SERVER
NetServerHandler::NetServerHandler(RemoteServer* r):
	handle(r)
{
	inThread = new std::thread([&]() {
		CGE::instance->logger->setThreadName("Net in");
		while (isRunning) {
			try {
				ByteBuffer packet;
				Socket::Address addr;
				handle->socket.read(packet, addr);
				if (addr.toLong() == handle->socket.remote.toLong()) {
					bps_current_in += packet.size();
					unsigned short header;
					packet >> header;
					bool compression = header & 1;
					Packet* p = CGE::instance->packetRegistry->instancePacket(header >> 1);
					if (p) {
						if (p->getPacketID() != 1) {
							//INFO(std::string("[RECI][") + std::to_string(p->getPacketID()) + "] " + Utils::classname(p) + " (" + std::to_string(packet.size()) + ")");
						}
						if (p->getPacketID() != 0x1b && !CGE::instance->thePlayer)
							continue;
						if (packet.available()) {
							ByteBuffer dec;
							if (compression) {
								Utils::decompress(packet, dec);
							}
							else
							{
								packet >> dec;
							}
							dec.seekg(0);
							p->read(dec);
						}
						else if (p->getPacketID() != 1)
						{
							WARN("Packet has no data.");
						}
						/*
						std::stringstream ss;
						ss << std::hex << packet;
						INFO(ss.str());*/

						++c_rec; 
						ts<IServer>::r lock = CGE::instance->server;
						if (CGE::instance->server) {
							lock->runOnServerThread([&, p]()
							{
								p->onReceived();
								delete p;
							});
							lastPacket = std::time(nullptr);
						}
					}
					else {
						std::stringstream ss;
						ss << "Unknown packet 0x" << std::hex << (header >> 1);
						throw CraftgameException(this, ss.str());
					}
				}
			}
			catch (CraftgameException e) {
				{
					std::string s = e.what();
					CGE::instance->uiThread.push([s] ()
					{
						CGE::instance->disconnect(s, "gui.disconnected.lostconnection");
					});
				}
				return;
			}
			catch (std::system_error e) {
				CGE::instance->logger->err(e.what());
				std::string s = e.what();
				CGE::instance->uiThread.push([s]()
				{
					CGE::instance->disconnect(s, "gui.disconnected.lostconnection");
				});
				return;
			}
		}
	});
	socketWatchdogThread = new boost::thread([&]()
	{
		while (isRunning)
		{
			boost::this_thread::sleep_for(boost::chrono::seconds(1));
			bps_in = bps_current_in.load();
			bps_out = bps_current_out.load();
			bps_current_in = bps_current_out = 0;
		}
	});
}

NetServerHandler::~NetServerHandler()
{
	isRunning = false;
	CGE::instance->threadPool.runAsync([](std::thread* inThread)
	{
		inThread->join();
		delete inThread;
	}, inThread);
	socketWatchdogThread->interrupt();
	socketWatchdogThread->join();
	delete socketWatchdogThread;
}

void NetServerHandler::sendPacket(std::shared_ptr<Packet> p)
{
	try {
		ByteBuffer buf;
		unsigned short header;
		ByteBuffer packet;
		p->write(packet);
		packet.seekg(0);
		//std::this_thread::sleep_for(std::chrono::milliseconds(70));
		size_t s = packet.size();
		header = p->getPacketID() << 1;
		if (s > 64)
		{
			header |= 1;
		}
		buf << header;
		if (s) {
			if (s > 64) {
				Utils::compress(packet, buf);
			}
			else
			{
				buf << packet;
			}
		}
		ts<IServer>::r lock(CGE::instance->server);
		if (p->getPacketID() != 1) {
			//INFO(std::string("[SENT][") + std::to_string(p->getPacketID()) + "] " + Utils::classname(p.get()) + " (" + std::to_string(buf.size()) + ")");
		}
		bps_current_out += buf.size();
		++c_snt;
		handle->socket.write(buf);
	}
	catch (std::exception& e) {
		std::string s = e.what();
		CGE::instance->uiThread.push([s]()
		{
			CGE::instance->disconnect(s);
		});
	}
	catch (...)
	{
		CGE::instance->uiThread.push([]()
		{
			CGE::instance->disconnect("Unknown error");
		});
	}
}

void NetServerHandler::sendPacket(Packet* p)
{
	sendPacket(std::shared_ptr<Packet>(p));
}

void NetServerHandler::getBPS(size_t& in, size_t& out)
{
	in = bps_in;
	out = bps_out;
}
#endif
