#pragma once
#include "Packet.h"
#include "AsyncQueue.h"


class RemoteServer;
namespace boost
{
	class thread;
};
class NetServerHandler
{
private:
	RemoteServer* handle;
	std::thread* inThread;
	boost::thread* socketWatchdogThread;
	std::mutex m;
	bool isRunning = true;
	std::atomic<size_t> bps_in = 0, bps_out = 0, bps_current_in = 0, bps_current_out = 0;
public:
	NetServerHandler(RemoteServer* _h);
	~NetServerHandler();
	std::time_t lastPacket = 0;
	void sendPacket(std::shared_ptr<Packet>);
	void sendPacket(Packet*);
	void getBPS(size_t& in, size_t& out);
	std::atomic<size_t> s_rec = 0, s_snt = 0; // Server recieved and sent packet count
	std::atomic<size_t> c_rec = 0, c_snt = 0; // Client recieved and sent packet count
	size_t p_loss_in = 0, p_loss_out = 0; // Packet loss
};