#include <boost/container/deque.hpp>
#include "GameEngine.h"
#include "CraftgameServer.h"
#include <boost/python/list.hpp>
#include <Windows.h>

namespace bpy = boost::python;
#ifndef SERVER	
extern HWND _hwnd;
#endif
void setTitle(std::string t)
{
#ifndef SERVER	
	t = "Craftgame: " + t;
	SetWindowText(_hwnd, t.c_str());
#endif
}
int getWidth()
{
#ifndef SERVER	
	return CGE::instance->width;
#else
	return 0;
#endif
}
int getHeight()
{
#ifndef SERVER
	return CGE::instance->height;
#else
	return 0;
#endif
}
void registerEventHandler(bpy::object mod, bpy::object handler, std::string name)
{
	CGE::instance->modManager->registerEventHandler(_R(name), CGE::instance->modManager->modByHandle(mod), handler);
}
void dgs(bpy::object& s)
{
#ifndef SERVER	
	CGE::instance->displayGuiScreen(bpy::extract<boost::shared_ptr<GuiScreen>>(s));
#endif
}
/*
void _setBlock(World& w, TileEntity t, Pos p)
{
	w.setBlock(new TileEntity(t), p);
}*/
bpy::list getPlayers()
{
	ts<IServer>::rw l(CGE::instance->server);
	if (CraftgameServer* s = dynamic_cast<CraftgameServer*>(l.get()))
	{
		// FIXME ÁËßÒÜ ÝÒÎ ÏÎËÍÀß ÕÓÉÍß
		bpy::list d;
		for (NetPlayerHandler* k : *s->getPlayers())
		{
			d.append(bpy::ptr(k));
		}
		return d;
	}
	return {};
}
bpy::list getWorlds()
{
	ts<IServer>::rw l(CGE::instance->server);
	if (CraftgameServer* s = dynamic_cast<CraftgameServer*>(l.get()))
	{
		// FIXME ÁËßÒÜ ÝÒÎ ÏÎËÍÀß ÕÓÉÍß
		bpy::list d;
		for (World* k : s->worlds)
		{
			d.append(bpy::ptr(k));
		}
		return d;
	}
	return {};
}
Block* getBlock(_R r)
{
	return CGE::instance->goRegistry->getBlock(r);
}
bpy::object getSide()
{
#ifdef SERVER
	bpy::object o(1);
#else
	bpy::object o(0);
#endif
	return o;
}

#ifndef SERVER
void cts(std::string url, unsigned short port)
{
	CGE::instance->connectToServer(url, port);
}
void disconnect()
{
	CGE::instance->disconnect();
}
#endif
class CGE_mod
{
public:
	const std::string modid;
	CGE_mod(const std::string& m);
	bpy::object call(bpy::object cl);
};

CGE_mod::CGE_mod(const std::string& m):
	modid(m)
{
}

void mod_init(bpy::object self)
{
	Mod* m = CGE::instance->modManager->modByHandle(self);
	if (m)
	{
		if (PythonMod* p = dynamic_cast<PythonMod*>(m))
		{
			if (p->state != PythonMod::HANDLE_TYPE)
			{
				throw std::runtime_error(m->modid + " mod already has handle");
			}
			p->state = PythonMod::HANDLE_MOD;
			p->handle = self;
			return;
		}
		throw std::runtime_error(m->modid + " is not PythonMod");
	}
	throw std::runtime_error("Mod does not exists");
}

bpy::object CGE_mod::call(bpy::object cl)
{
	Mod* m = CGE::instance->modManager->getModById(modid);
	if (m)
	{
		if (PythonMod* p = dynamic_cast<PythonMod*>(m))
		{
			if (p->state != PythonMod::HANDLE_MODULE)
			{
				throw std::runtime_error(modid + " mod already has handle");
			}
			cl.attr("__init__") = bpy::make_function(&mod_init);
			p->handle = cl;
			p->state = PythonMod::HANDLE_TYPE;
			return cl;
		}
		throw std::runtime_error(modid + " is not PythonMod");
	}
	throw std::runtime_error(modid + " mod does not exists");
}
