#pragma once

#include "EntityPlayerMP.h"
#include "Socket.h"
#include "ThreadPool.h"

class NetPlayerHandler {
public:
	NetPlayerHandler(EntityPlayerMP*, Socket*);
	EntityPlayerMP* player;
	Socket* socket;
	ThreadPool t;
};