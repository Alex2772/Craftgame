#include "NetPlayerHandler.h"
#include "GameEngine.h"
#include "ByteBuffer.h"
#include "P04ServerData.h"
#include "P00Disconnected.h"
#include "Utils.h"
#include "CraftgameServer.h"
#include "PHandshake.h"

NetPlayerHandler::NetPlayerHandler(Socket::Address& s):
	address(s),
	commandSender(nullptr)
{
	lastPacket = std::time(nullptr);
}

NetPlayerHandler::~NetPlayerHandler()
{
	isRunning = false;
	if (player)
		delete player;
	ts<IServer>::rw lock(CGE::instance->server);
	dynamic_cast<CraftgameServer*>(lock.get())->removeClient(this);

}

void NetPlayerHandler::disconnect(const std::string& cs)
{
	INFO(std::string("Disconnected ") + (player ? player->getGameProfile().getUsername() : address.toString()) + ": " + cs);
	sendPacket(new P00Disconnected(cs));
	ts<IServer>::rw(CGE::instance->server)->dropPlayer(player->getId());
}

Socket::Address& NetPlayerHandler::getAddress()
{
	return address;
}

EntityPlayerMP* NetPlayerHandler::getPlayer()
{
	return player;
}


/**
 * \brief Увеличивает значение MPS (chat Messages Per Second). При достижении определённого значения кикает с сервера.
 */
void NetPlayerHandler::increaseMPS()
{
	mps++;
	if (mps > 2)
	{
		//CGE::instance->getCEServer()->disconnectPlayer(player->getId(), "gui.disconnected.spam");
	}
}

/**
 * \brief Вызывать эту функцию раз в секунду.
 */
void NetPlayerHandler::secTick()
{
	if (mps)
		mps--;
}

short NetPlayerHandler::getPing() const
{
	return ping;
}

void NetPlayerHandler::setPing(short p)
{
	ping = p;
	if (stats_lck.try_lock())
	{
		uint16_t snt = s_snt, rec = s_rec;
		s_snt = 0;
		s_rec = 0;
		stats_lck.unlock();
		P04ServerData* p = new P04ServerData(4, player);
		p->p_snt = snt;
		p->p_rec = rec;
		sendPacket(p);
	}
	sendPacket((new P04ServerData(2, player))->setPing(ping));
}

void NetPlayerHandler::sendPacket(std::shared_ptr<Packet> p)
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
		{
			std::unique_lock<std::mutex> lock(stats_lck);
			++s_snt;
		}
		ts<IServer>::r lock(CGE::instance->server);
		if (p->getPacketID() != 1) {
			//INFO(std::string("[SENT][") + std::to_string(p->getPacketID()) + "] " + Utils::classname(p.get()) + " (" + std::to_string(buf.size()) + ")");
		}

		dynamic_cast<CraftgameServer*>(lock.get())->getSocket().write(buf, address);
	} catch(std::exception& e)
	{
		disconnect(e.what());
	}
	catch (...)
	{
		ts<IServer>::r(CGE::instance->server)->dropPlayer(player->getId());
	}
}

void NetPlayerHandler::sendPacket(Packet* p)
{
	sendPacket(std::shared_ptr<Packet>(p));
}

void NetPlayerHandler::handlePacket(ByteBuffer& buf)
{
	if (handshakeDone) {
		try {
			if (buf.size() > 256) // Клиентам большего и не нужно
			{
				throw CraftgameException(this, "Too long data packet size");
			}
			unsigned short header;
			buf >> header;
			unsigned short packet = header >> 1;
			bool compression = header & 1;
			Packet* p = CGE::instance->packetRegistry->instancePacket(packet);
			if (p) {
				if (p->getPacketID() != 1)
				{
					//INFO(std::string("[RECI][") + std::to_string(p->getPacketID()) + "] " + Utils::classname(p) + " (" + std::to_string(buf.size()) + ")");
				}
				if (buf.available()) {
					ByteBuffer dec;
					if (compression) {
						Utils::decompress(buf, dec);
					}
					else
					{
						buf >> dec;
					}
					dec.seekg(0);
					p->read(dec);
				}

				++dllock;
				CGE::instance->threadPool.runAsync([&, p]()
				{
					p->onReceived(player);
					delete p;
					--dllock;
				});
				lastPacket = std::time(nullptr);
				std::unique_lock<std::mutex> gr4ergrtgrtggt(stats_lck);
				++s_rec;
			}
			else {
				std::stringstream ss;
				ss << "Unknown packet 0x" << std::hex << packet;
				throw CraftgameException(this, ss.str());
			}
		}
		catch (CraftgameException e) {
			CGE::instance->logger->err(e.what());
			ts<IServer>::r(CGE::instance->server)->dropPlayer(player->getId());
		}
		catch (std::system_error e) {
			CGE::instance->logger->err(e.what());
			ts<IServer>::r(CGE::instance->server)->dropPlayer(player->getId());
			return;
		}
	} else
	{
		try
		{
			byte b;
			buf >> b;
			if (b != 0x3c)
				throw std::runtime_error("Control byte is incorrect");
			std::string username;
			buf >> username;
			if (username.length() < 4)
			{
				throw std::runtime_error("Nickname is too short");
			}
			if (username.length() > 15)
			{
				throw std::runtime_error("Nickname is too long");
			}
			if (ts<IServer>::r(CGE::instance->server)->getPlayer(username) != nullptr)
			{
				throw  std::runtime_error("This player is already playing on this server");
			}

			size_t id = CGE::instance->assignGlobalUniqueEntityId();
			player = new EntityPlayerMP(nullptr, id, GameProfile(username));
			player->setNetHandler(this);
			commandSender = PlayerCommandSender(player);
			ts<IServer>::r lock(CGE::instance->server);
			dynamic_cast<CraftgameServer*>(lock.get())->getPlayers()->push_back(this);
			
			INFO(username + " joined the game with entity ID " + std::to_string(id));
			CGE::instance->chat->broadcast(std::string("§e") + username + " joined the game.");
			PHandshake* p = new PHandshake();
			p->info = ts<IServer>::r(CGE::instance->server)->info;
			p->id = id;
			p->adt = player->adtTag;
			sendPacket(p);
			handshakeDone = true;
		} catch (std::exception& e)
		{
			WARN(std::string("Drop ") + address.toString() + ": handshake error: " + e.what());
			delete this;
		}
		catch (...)
		{
			WARN(std::string("Drop ") + address.toString() + ": handshake error");
			delete this;
		}
	}
}
