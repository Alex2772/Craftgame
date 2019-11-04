#include "CraftgameServer.h"
#include "GameEngine.h"
#include "P01Ping.h"
#include "P04ServerData.h"
#include "PSpawnEntity.h"
#include "PWorldTime.h"
#include "PEntityLook.h"
#include "PItemHeld.h"

CraftgameServer::CraftgameServer():
serverSocket(24565)
{
	worlds.push_back(new World);
	connectionsThread = std::thread([&]()
	{
		CGE::instance->logger->setThreadName("Server net in");
		clients.setObject(new std::map<Socket::Address, NetPlayerHandler*>);
		while (isRunning)
		{
			try {
				ByteBuffer data;
				Socket::Address addr;
				serverSocket.read(data, addr);
				{
					Clients::r k = clients;
					auto it = k->find(addr);
					if (it != k->end())
					{
						it->second->handlePacket(data);
						continue;
					}
				}
				INFO(std::string("Attemping connection from ") + addr.toString());
				Clients::rw x = clients;
				NetPlayerHandler* h = new NetPlayerHandler(addr);
				(*x.get())[addr] = h;
				h->handlePacket(data);
			} catch(std::exception& e)
			{
				WARN(e.what());
			}
			/*
			Socket* s = serverSocket.accept();
			INFO(std::string("Attemping connection from ") + std::to_string(s->server.sin_addr.S_un.S_un_b.s_b1) + "." + std::to_string(s->server.sin_addr.S_un.S_un_b.s_b2) + "." + std::to_string(s->server.sin_addr.S_un.S_un_b.s_b3) + "." + std::to_string(s->server.sin_addr.S_un.S_un_b.s_b4));
			try {
				if (s->read<byte>() == 0x3c)
				{
					s->write(info);

					// Ник
					std::string username;
					{
						size_t t = s->read<size_t>();
						if (t > 15) {
							s->write<byte>(2); // Слишком длинный ник
							delete s;
							continue;
						}
						char* buffer = new char[t];
						s->readBytes(buffer, t, 0);
						username = std::string(buffer, t);
						delete[] buffer;
					}
					
					// Проверка играет ли игрок с таким же ником
					for (Players::iterator i = players.begin(); i != players.end(); ++i)
					{
						if ((*i)->player->getGameProfile().getUsername() == username)
						{
							s->write<byte>(1);
							delete s;
							INFO(std::string("Disconnected ") + username + ": this player is already playing on this server");
							return;
						}
					}

					size_t id = CGE::instance->assignGlobalUniqueEntityId();
					s->write<byte>(0);
					s->write<size_t>(id);
					EntityPlayerMP* e = new EntityPlayerMP(nullptr, id, GameProfile(username));
					ByteBuffer adt;
					ADT::serialize(adt, e->adtTag);
					s->write<size_t>(adt.size());
					s->writeBytes(adt.get(), adt.size());
					players.push_back(new NetPlayerHandler(e, s));
					
					INFO(username + " joined the game with entity ID " + std::to_string(id));
					CGE::instance->chat->broadcast(std::string("§e") + username + " joined the game.");
				} else
				{
					delete s;
				}
			} catch(CraftgameException e)
			{
				//delete s;
			}*/
		}
	});
	tickrateWatchdog = std::thread([&]()
	{
		CGE::instance->logger->setThreadName("Tickrate watchdog");
		INFO("Tickrate watchdog started");
		while (isRunning)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			currentTickrate = ticks;
			ticks = 0;
		}
	});
	chunkSelect = std::thread([&]()
	{
		while (isRunning)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			//ts<IServer>::r lock(CGE::instance->server);
			for (NetPlayerHandler* cl : players)
			{
				if (cl->getPlayer()->getPrevPos() != cl->getPlayer()->getPos())
				{
					cl->getPlayer()->selectChunks();
				}
			}
		}
	});
}
CraftgameServer::~CraftgameServer()
{
	close();
}

void CraftgameServer::close()
{
	if (isRunning) {
		isRunning = false;
		serverSocket.close();
		connectionsThread.join();
		tickrateWatchdog.join();
		chunkSelect.join();
		for (size_t i = 0; i < players.size(); i++)
		{
			delete players[i];
		}
		players.clear();
		for (size_t i = 0; i < worlds.size(); i++)
			delete worlds[i];
		worlds.clear();
	}
}

void CraftgameServer::dropPlayer(const size_t id)
{
	runOnServerThread([&, id]()
	{
		for (Players::iterator i = players.begin(); i != players.end(); ++i)
		{
			NetPlayerHandler* n = *i;
			if (n->player->getId() == id)
			{
				players.erase(i);
				n->player->remove();
				CGE::instance->chat->broadcast(std::string("§e") + n->player->getGameProfile().getUsername() + " left the game.");
				sendPacket(new P04ServerData(1, n->player));
				ts<IServer>::rw lock(CGE::instance->server);
				delete n;
				return;
			}
		}
	});
}

EntityPlayerMP* CraftgameServer::getPlayer(const size_t& id)
{
	for (Players::iterator i = players.begin(); i != players.end(); ++i)
	{
		NetPlayerHandler* n = *i;
		if (n->player->getId() == id)
		{
			return n->player;
		}
	}
	return nullptr;
}

EntityPlayerMP* CraftgameServer::getPlayer(std::string name)
{
	for (Players::iterator i = players.begin(); i != players.end(); ++i)
	{
		NetPlayerHandler* n = *i;
		if (n->player->getGameProfile().getUsername() == name)
		{
			return n->player;
		}
	}
	return nullptr;
}
void CraftgameServer::runOnServerThread(std::function<void()> f)
{
	serverThreadFunctions.push(f);
}

void CraftgameServer::sendPacket(Packet* p)
{
	runOnServerThread([&, p] ()
	{
		std::shared_ptr<Packet> pack(p);
		for (Players::iterator i = players.begin(); i != players.end(); ++i)
		{
			NetPlayerHandler* net = *i;
			if (p->getPacketID() == 0x01)
				net->sendPingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
			net->sendPacket(pack);
		}
	});
}

void CraftgameServer::sendPacketAllExcept(Packet* p, NetPlayerHandler* n)
{
	runOnServerThread([&, n, p]()
	{
		std::shared_ptr<Packet> pack(p);
		for (Players::iterator i = players.begin(); i != players.end(); ++i)
		{
			NetPlayerHandler* net = *i;
			if (net == n)
				continue;
			if (p->getPacketID() == 0x01)
				net->sendPingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
			net->sendPacket(pack);
		}
	});
}

void CraftgameServer::runServer()
{
	while (!serverThreadFunctions.empty())
	{
		std::function<void()> f = serverThreadFunctions.front();
		serverThreadFunctions.pop();
		f();
	}
}

Socket& CraftgameServer::getSocket()
{
	return serverSocket;
}

void CraftgameServer::run()
{
	while (isRunning)
	{
		auto _st = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());


		if (tick == 0)
		{
			sendPacket(new P01Ping);
			for (std::deque<NetPlayerHandler*>::iterator i = players.begin(); i != players.end(); i++)
			{
				if (time(0) - (*i)->lastPacket > 30)
				{
					(*i)->disconnect("cge.error.timeout");
				} else
				{
					World* w = (*i)->getPlayer()->worldObj;
					if (w)
						(*i)->sendPacket(new PWorldTime(w->time));
				}
			}
		}
		{
			//ts<IServer>::r lock(CGE::instance->server);
			runServer();
			CGE_EVENT("tick", , []()
			{

			});
			for (size_t i = 0; i < worlds.size(); i++)
			{
				worlds[i]->tick();
			}
		}

		tick++;
		ticks++;
		tick %= 20;

		auto _st2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()) - _st;
		
		uint16_t c = (1000 / maxTickrate);
		if (_st2.count() < c) {
			std::this_thread::sleep_for(std::chrono::milliseconds(c - _st2.count()));
			CGE::instance->millis = 1.f / float(maxTickrate);
		} else
		{
			CGE::instance->millis = float(_st2.count()) / 1000.f;
		}
	}
}

void CraftgameServer::removeClient(NetPlayerHandler* n)
{
	Clients::rw k = clients;
	k->erase(k->find(n->getAddress()));
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
					//arg.data = new char[1];
					arg.type = T_FLAG;
					arg.clear();
					try {
						if (elem.find(".") != std::string::npos)
						{
							float data = stof(elem);
							arg.type = T_FLOAT;
							arg.dataSize = sizeof(data);
							arg.data = new char[sizeof(data)];
							memcpy(arg.data, reinterpret_cast<char*>(&data), sizeof(data));
						}
						else {
							int data = stoi(elem);
							arg.type = T_INTEGER;
							arg.dataSize = sizeof(data);
							arg.data = new char[sizeof(data)];
							memcpy(arg.data, reinterpret_cast<char*>(&data), sizeof(data));
						}
					}
					catch (std::invalid_argument e) {
						arg.dataSize = elem.size() + 1;
						arg.type = T_STRING;
						arg.data = new char[elem.size() + 1];
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
				size_t j = 0;
				for (size_t i = 0; i < at.size(); i++) {
					if (at[i].n.size()) {
						size_t j = 0;
						for (; j < vect.size(); j++) {
							if (at[i].n == vect[j].name) {
								if (!(at[i].type & vect[j].type)) {
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
					}
					else {
						for (; j < vect.size(); j++) {
							{
								if (at[i].type & vect[j].type) {
									if (at[i].need) {
										fndp++;
										j++;
										break;
									}
								}
								else {
									sender->sendMessage(cmd->constructCommandUsage());
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
						cmd->execute(sender, a);
					}
					catch (CraftgameException e) {
						sender->sendMessage(std::string("An error has occurred while executing command ") + cmd->getName() + ": " + e.what());
					}
				});

			}
			catch (CraftgameException e) {
				sender->sendMessage(std::string("An error has occurred while executing command ") + cmd->getName() + ": " + e.what());
			}
		}
		else {
			sender->sendMessage(std::string("Unknown command ") + s + ". Type help to get help");
		}
	}
}

void CraftgameServer::playerJoinWorld(EntityPlayerMP* e)
{
	Pos t = worlds[0]->getHighestPoint(e->getPos().x, e->getPos().z);
	worlds[0]->spawnEntity(e, DPos(0, 0, 0));
	for (std::vector<Entity*>::iterator i = worlds[0]->entities.begin(); i != worlds[0]->entities.end(); ++i)
	{
		e->getNetHandler()->sendPacket(new PSpawnEntity(*i));
		if (EntityPlayerMP* p = dynamic_cast<EntityPlayerMP*>(*i))
		{
			if (p->getHeldItem())
			{
				e->getNetHandler()->sendPacket(new PItemHeld(p, p->getHeldItem()));
			}
		}
	}
	e->setPos(DPos(t.x, t.y + 1, t.z));
}

void CraftgameServer::sendPacketAround(Packet* p, const DPos& pos, float radius)
{
	runOnServerThread([&, p]()
	{
		std::shared_ptr<Packet> pack(p);
		for (Players::iterator i = players.begin(); i != players.end(); ++i)
		{
			NetPlayerHandler* net = *i;
			if (glm::length(net->getPlayer()->getPos().toVec3() - pos.toVec3()) <= radius)
				net->sendPacket(pack);
		}
	});
}