#include "ModManager.h"
#include "GameEngine.h"
#include "Utils.h"

Mod* ModManager::getModById(const std::string& modid)
{
	for (size_t i = 0; i < CGE::instance->modManager->mods.size(); i++)
	{
		Mod* m = CGE::instance->modManager->mods[i];
		if (m->modid == modid)
		{
			return m;
		}
	}
	return nullptr;
}

bool ModManager::pythonEvent(PythonEvent* e)
{
	tvec& h = getHandler(e->name);
	for (tvec::iterator i = h.begin(); i != h.end(); i++)
	{
		try {
			i->callable(python::ptr(e));
			if (e->isCancelled())
				return false;
		}
		catch (const boost::python::error_already_set&)
		{
			CGE::instance->logger->err(std::string("An error has occurred in ") + i->mod->modid + " while processing " + e->name.full + " event");
			CGE::instance->logger->err("Python error:");
			std::string str = Utils::pythonErrorString();
			CGE::instance->logger->err(str);
		}
	}
	return true;
}
