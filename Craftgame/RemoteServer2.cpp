#ifndef SERVER
#include "RemoteServer.h"
#include "GameEngine.h"

void RemoteServer::threadFunc()
{
	while (isRunning)
	{
		auto _st = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
		{
			ts<IServer>::rw l(CGE::instance->server);
			while (!serverThreadFunctions.empty())
			{
				serverThreadFunctions.front()();
				serverThreadFunctions.pop();
			}

			if (CGE::instance && CGE::instance->thePlayer && CGE::instance->thePlayer->worldObj)
				CGE::instance->thePlayer->worldObj->tick();

			CGE_EVENT("tick", , []()
			{

			});
		}

		auto _st2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()) - _st;


		if (_st2.count() < 50) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50 - _st2.count()));
		}
	}
}
#endif