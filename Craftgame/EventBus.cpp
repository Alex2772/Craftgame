#include "EventBus.h"
#include "ModManager.h"
#include "GameEngine.h"
bool EventBus::pre(Event* e)
{
	Resource res = e->name;
	e->name = (res.full + "/pre");
	bool r = process(e);
	e->name = res;
	return r;
}
bool EventBus::ev(Event* e)
{
	return process(e);
}
void EventBus::post(Event* e)
{
	Resource res = e->name;
	e->name = (res.full + "/post");
	process(e);
	e->name = res;
}
bool EventBus::process(Event* e)
{
	PythonEvent* pe;
	if (pe = dynamic_cast<PythonEvent*>(e))
	{
		return CGE::instance->modManager->pythonEvent(pe);
	}
	return true;
}

PythonEvent* PythonEvent::param(std::string s, std::string str)
{
	std::wstring w;
	if (!str.empty()) {
		int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
		w = std::wstring(size_needed, 0);
		MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &w[0], size_needed);
	}
	dict[s] = boost::python::str(w);
	return this;
}