  #include "CraftgameServer.h"
#include "GameEngine.h"
#include "ByteBuffer.h"
#include "glm/glm.hpp"
#include "OS.h"

CraftgameServer::CraftgameServer() {
	serverSocket = new ServerSocket(24565);
	ccs = new ConsoleCommandSender;
	INFO("Starting Craftgame server on port 24565");

	connectThread = new thread([&]() {
		while (isRunning) {
			Socket* s = nullptr;
			try {
				s = serverSocket->accept();
				if (!s || !CGE::instance->server)
					return;
				CGE::instance->threadPool.runAsync([&, s]() {
					try {
						if (s) {
							char b;
							s->readBytes(&b, 1, 0);
							if (CGE::instance->server && b == 0x3c) {
								s->write(0xac);
								if (s->read<size_t>() != VERSION_CODE) {
									char c = 0xf0;
									s->write(c);
									s->close();
									delete s;
									return;
								}
								ID_sck d;
								d.id = CGE::instance->assignGlobalUniqueEntityId();
								d.socket = s;
								d.joinTime = std::time(0);
								std::unique_lock<std::mutex>(sockets.mutex);
								sockets.push_back(d);
								s->write(d.id);
							}
							else {
								s->close();
								delete s;
							}
						}

					}
					catch ( CraftgameException e) {
						s->close();
						delete s;
						CGE::instance->logger->err(e.what());

					}
				});
			}
			catch ( CraftgameException e) {
				if (s) {
					s->close();
					delete s;
				}
				CGE::instance->logger->err(e.what());
				
			}
		}
	});
	connectionsWatcher = new thread([&]() {
		while (isRunning) {
			size_t size;
			for (size_t i = 0; i < sockets.size() && connectThread;) {
				std::unique_lock<std::mutex>(sockets.mutex);
				if (sockets[i].joinTime + 10 < std::time(0)) {
					sockets.erase(sockets.begin() + i);
				}
				else {
					Socket* s = sockets[i].socket;
					try {
						unsigned short packet = s->read<unsigned short>();
						Packet* p = CGE::instance->packetRegistry->getPacket(packet);
						if (p) {
							ByteBuffer bb;
							size_t size = s->read<size_t>();
							char* b = new char[size];
							s->readBytes(b, size, 0);
							bb.put(b, size);
							delete[] b;
							p->read(bb);
							p->onReceived(nullptr);
						}
						else {
							throw  CraftgameException();
						}
					}
					catch ( CraftgameException e) {
						delete s;
						sockets.erase(sockets.begin() + i);
					}
					i++;
				}
			}
			OS::sleep(10);
		}
	});
	worlds.push_back(new World);
}
void CraftgameServer::close() {
	isRunning = false;
	if (serverSocket) {
		delete serverSocket;
		serverSocket = nullptr;
	}
	if (connectThread) {
		connectThread->join();
		delete connectThread;
		connectThread = nullptr;
	}
	if (connectionsWatcher) {
		connectionsWatcher->join();
		delete connectionsWatcher;
		connectionsWatcher = nullptr;
	}
	for (size_t i = 0; i < sockets.size(); i++) {
		if (sockets[i].socket)
			delete sockets[i].socket;
	}
	if (ccs)
		delete ccs;

	sockets.clear();
	for (size_t i = 0; i < worlds.size(); i++) {
		delete worlds[i];
	}
	worlds.clear();
}
CraftgameServer::~CraftgameServer() {
	close();
}
void CraftgameServer::sendPacket(Packet* packet) {
	CGE::instance->threadPool.runAsync([&, packet]() {
		
		ByteBuffer bb;
		packet->write(bb);
		for (size_t i = 0; i < players.size(); i++) {
			NetPlayerHandler w = players[i];
			w.t.runAsync([&, w, i]() {
				try {
					Socket* socket = w.socket;
					socket->write(packet->getPacketID());
					socket->write(bb.size());
					if (bb.size())
						socket->writeBytes(bb.get(), bb.size());
				}
				catch ( CraftgameException e) {
					((CraftgameServer*)CGE::instance->server)->disconnectPlayer(w.player->getId(), "An error has occurred while sending packet; this connection has been terminated");
				}
				if (i + 1 < players.size()) {
					delete packet;
					return;
				}
			});
			
		}
	});

}
void CraftgameServer::sendPacketTo(Packet* packet, EntityPlayerMP* pl) {
	try {
		sendPacketTo(packet, pl->getSocket());
	}
	catch ( CraftgameException e) {
		((CraftgameServer*)CGE::instance->server)->disconnectPlayer(pl->getId(), "An error has occurred while sending packet; this connection has been terminated");
	}
}
void CraftgameServer::sendPacketTo(Packet* packet, Socket* socket) {
	ByteBuffer bb;
	socket->write(packet->getPacketID());
	socket->write(bb.size());
	socket->writeBytes(bb.get(), bb.size());
}
Packet* CraftgameServer::readPacket() {
	return nullptr;
}
void CraftgameServer::runOnServerThread(std::function<void()> func) {
	serverThreadF.push(func);
}
void CraftgameServer::tick() {
	if (serverThreadF.size()) {
		serverThreadF.front()();
		serverThreadF.pop();
	}
}

#include "PacketDisconnected.h"
void CraftgameServer::disconnectPlayer(size_t entityId, std::string reason) {
	for (size_t i = 0; i < sockets.size();) {
		if (sockets[i].id == entityId) {
			try {
				sendPacketTo(new PacketDisconnected(reason), sockets[i].socket);
				delete sockets[i].socket;
				sockets.erase(sockets.begin() + i);
			}
			catch (...) {}
		}
		else {
			i++;
		}
	}
}

void CraftgameServer::initiateShutdown()
{
	isRunning = false;
}

void CraftgameServer::run()
{
	while (isRunning) {
		try {
			tick();
		}
		catch (CraftgameException e) {
			CGE::instance->logger->err("Caught an exception while ticking server");
			CGE::instance->logger->err(e.what());
			break;
		}
	}
}

void CraftgameServer::dispatchCommand(CommandSender* sender, std::string& s) {
	std::vector<CmdLineArg> vect;
	std::vector<std::string> a = Parsing::splitString(s, " ");
	if (a.size()) {
		if (Command* cmd = CGE::instance->commandRegistry->findCommandByName(a[0])) {
			try {
				for (size_t i = 1; i < a.size(); i++) {
					std::string elem = a[i];
					CmdLineArg arg;
					arg.name = new char[1];
					arg.name[0] = '\0';
					arg.data = new char[1];
					arg.data[0] = '\0';
					arg.dataSize = 1;
					arg.type = T_FLAG;
					if (Parsing::startsWith(elem, "-")) {
						arg.name = elem.substr(1);
						if (a.size() > i + 1) {
							elem = a[i + 1];
							i++;
							if (Parsing::startsWith(elem, "-")) {
								vect.push_back(arg);
								continue;
							}
						}
						else {
							vect.push_back(arg);
							continue;
						}
					}
					arg.data = new char[elem.size() + 1];
					try {
						int data = stoi(elem);
						arg.type = T_INTEGER;
						std::string s = std::to_string(data);
						arg.dataSize = s.size() + 1;
						memcpy(arg.data, s.c_str(), s.size() + 1);
					}
					catch (std::invalid_argument e) {
						arg.dataSize = elem.size() + 1;
						arg.type = T_STRING;
						memcpy(arg.data, elem.c_str(), elem.size() + 1);
					}
					vect.push_back(arg);
				}
				std::vector<Arg> at = cmd->getArgTypes();
				size_t ndp = 0;

				for (size_t i = 0; i < at.size(); i++) {
					if (at[i].need)
						ndp++;
				}
				size_t fndp = 0;
				size_t counter = 0;
				size_t j = 0;
				for (size_t i = 0; i < at.size(); i++) {
					if (at[i].n.size()) {
						size_t j = 0;
						for (; j < vect.size(); j++) {
							if (at[i].n == vect[j].name) {
								if (at[i].type != vect[j].type) {
									sender->sendMessage(cmd->constructCommandUsage());
									return;
								}
								else {
									if (at[i].need)
										fndp++;
									break;
								}
							}
						}
					} else {
						if (at.size() > counter) {
							for (; j < vect.size(); j++) {
								if (vect[j].name.size())
									counter++;
								if (j + 1 == counter) {
									if (vect[j].name.size() && at[i].type == vect[j].type) {
										if (at[i].need) {
											fndp++;
											j++;
											break;
										}
									} else {
										sender->sendMessage(cmd->constructCommandUsage());
									}
								}
							}
						}
					}
				}
				if (fndp != ndp) {
					sender->sendMessage(cmd->constructCommandUsage());
					return;
				}
				runOnServerThread([&, sender, cmd, vect]() {
					try {
						std::vector<CmdLineArg> a = vect;
						cmd->execute(ccs, a);
					}
						catch (CraftgameException e) {
						sender->sendMessage(std::string("An error has occurred while executing command ") + cmd->getName() + ": " + e.what());
					}
				});

			}
			catch ( CraftgameException e) {
				sender->sendMessage(std::string("An error has occurred while executing command ") + cmd->getName() + ": " + e.what());
			}
		}
		else {
			sender->sendMessage(std::string("Unknown command ") + s + ". Type help to get help");
		}
	}
}
