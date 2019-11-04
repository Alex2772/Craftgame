#include "Res.h"
#include <fstream>
#include "CraftgameException.h"
#include <memory>
#include "Socket.h"
#include "GuiScreenLoad.h"
#include "GameEngine.h"

Resource::Resource(std::string domain, std::string path) {
	Resource::domain = domain;
	Resource::path = path;
	Resource::full = domain + ":" + path;
}
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
}
Resource::Resource(std::string full) {
	Resource::full = full;
	std::vector<std::string> splt;
	split(full, ':', splt);
	if (splt.size() == 2) {
		Resource::domain = splt[0];
		Resource::path = splt[1];
	}
	else {
		Resource::domain = "craftgame";
		Resource::path = full;
		Resource::full = Resource::domain + ":" + full;
	}
}

Resource::Resource(const char* full):
	Resource(std::string(full))
{

}

std::shared_ptr<std::istream> Resource::open()
{
	if (domain == "craftgame")
	{
		std::string s = path;
		CGE::instance->uiThread.push([&, s]()
		{
#ifndef SERVER
			if (!CGE::instance->guiScreens.empty()) {
				GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
				if (ls)
				{
					ls->setDisplayStringAsync(CGE::instance->currentLocale->localize("gui.load.asset", spair("asset", vr(s))));
				}
			}
#endif
		});
		return std::make_shared<std::ifstream>(path, std::ios::binary);
	} else if (domain == "http" || domain == "https")
	{
		std::shared_ptr<std::stringstream> ss = std::make_shared<std::stringstream>();
		http_get(full, ss.get());
		ss->seekg(0);
		return ss;
	}
	else {
		std::map<std::string, IStreamProvider*>::iterator k = _kek.find(domain);
		if (k != _kek.end())
		{
			std::shared_ptr<std::istream> ss = k->second->openStream(path);
			return ss;
		}
		else
		{
			throw CraftgameException(this, std::string("Domain ") + domain + " is not supported.");
		}
	}
}

std::map<std::string, Resource::IStreamProvider*> Resource::_kek;

void Resource::registerIStreamProvider(std::string ns, IStreamProvider* isp)
{
	if (ns == "craftgame" || ns == "http" || ns == "https" || _kek.find(ns) != _kek.end())
		throw std::runtime_error("Namespace " + ns + " is already registered");

	_kek[ns] = isp;
}


