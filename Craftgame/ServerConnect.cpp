#ifndef SERVER
#include "ServerConnect.h"
#include "GameEngine.h"
#include "GuiScreenMessageDialog.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include "RemoteServer.h"
#include "AnimationFade.h"
#include "P02JoinWorld.h"
#include "GuiScreenMainMenu.h"

extern GuiScreenMessageDialog* _connectionDialog;

void ServerConnect::connectToServer(std::string url, unsigned short port)
{
	_connectionDialog = new GuiScreenMessageDialog(_("gui.connecting.connecting"), _("gui.connecting.connecting.desc"), [](int)
	{
	}, 0);
	CGE::instance->displayGuiScreen(_connectionDialog);
	boost::shared_ptr<boost::thread> thread(new boost::thread([&, url, port]()
	{
		srand(time(0));
		RemoteServer* r = nullptr;
		ts<IServer>::rw lock(CGE::instance->server);
		try {
			r = new RemoteServer(url, port);
			CGE::instance->server.setObject(r);
			r->net = new NetServerHandler(r);
			ByteBuffer handshake;
			
			handshake.put<byte>(0x3c);
			handshake << CGE::instance->gameProfile->getUsername();
			r->socket.write(handshake);

			boost::this_thread::sleep_for(boost::chrono::seconds(1));
			boost::this_thread::interruption_point();

			CGE::instance->uiThread.push([&]()
			{
				for (size_t i = CGE::instance->guiScreens.size(); i > 0; --i)
				{
					if (CGE::instance->guiScreens[i - 1].get() != _connectionDialog) {
						Animation* a = new AnimationFade(CGE::instance->guiScreens[i - 1].get());
						CGE::instance->guiScreens[i - 1]->animations.push_back(a);
						CGE::instance->guiScreens[i - 1]->animatedClose(a);
					}
				}
			});
		}
		catch (std::exception e)
		{
			if (CGE::instance->server)
			{
				CGE::instance->server.destruct();
			}
			else if (r)
			{
				delete r;
			}
			std::string er = e.what();
			CGE::instance->uiThread.push([&, er]()
			{
				_connectionDialog->close();
				_connectionDialog = nullptr;
				if (CGE::instance->guiScreens.size() < 2)
				{
					CGE::instance->displayGuiScreen(new GuiScreenMainMenu());
				}
				CGE::instance->displayGuiScreen(new GuiScreenMessageDialog(_("gui.disconnected.lostconnection"), er));
			});
		} catch (boost::thread_interrupted& e)
		{
		}
		
	}));

	_connectionDialog->addButton(new GuiButton(_("gui.cancel"), [thread](int, int, int)
	{
		_connectionDialog->close();
		_connectionDialog = nullptr;
		thread->interrupt();
		thread->join();
		CGE::instance->disconnect();
	}));
}
#endif