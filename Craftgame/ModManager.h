#pragma once
#include "Res.h"
#include <functional>
#include <boost/python.hpp>
#include <map>
#include <vector>
#include "EventBus.h"
#include "Mod.h"
#include "PythonMod.h"

class ModManager
{
private:
	struct ModHandler
	{
		Mod* mod;
		boost::python::object callable;
	};
	typedef std::vector<ModHandler> tvec;
	typedef std::map<_R, tvec> tmap;
	tmap handlers;
	tvec& getHandler(_R& r)
	{
		tmap::iterator f = handlers.find(r);
		if (f != handlers.end())
		{
			return f->second;
		}
		handlers[r] = tvec();
		return handlers[r];
	}
public:
	std::string currentFile;
	bool initiated = false;
	ModManager()
	{
		
	}
	~ModManager()
	{
		while (!mods.empty()) {
			delete mods.back();
			mods.pop_back();
		}
	}

	Mod* getModById(const std::string& cs);
	std::vector<Mod*> mods;
	void registerEventHandler(_R eventName, Mod* o, boost::python::object callable)
	{
		getHandler(eventName).push_back({o, callable });
	}
	bool pythonEvent(PythonEvent* e);

	Mod* modByHandle(const python::object& mod)
	{
		for (size_t i = 0; i < mods.size(); i++)
		{
			PythonMod* m = dynamic_cast<PythonMod*>(mods[i]);
			switch (m->state) {
			case PythonMod::HANDLE_TYPE:
				if (m && m->handle.ptr() == reinterpret_cast<PyObject*>(mod.ptr()->ob_type))
				{
					return m;
				}
				break;
			case PythonMod::HANDLE_MOD:
				if (m->handle.ptr() == mod.ptr())
				{
					return m;
				}
			}
		}
		return nullptr;
	}
};
