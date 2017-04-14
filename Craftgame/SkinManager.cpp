#ifndef SERVER
#include "SkinManager.h"
#include "GameEngine.h"
#include "Socket.h"
#include <functional>
#include "ClientOnly.h"
#include "Exceptions.h"

SkinManager::SkinManager() {}
SkinManager::~SkinManager() {
	for (Skins::iterator i = skins.begin(); i != skins.end(); i++) {
		delete i->second;
	}
	skins.clear();
}

size_t* SkinManager::getSkin(std::string name) {
	static size_t ds = CGE::instance->textureManager->loadTexture("res/entity/default.png");
	
	Skins::iterator it = skins.find(name);
	if (it != skins.end()) {
		return it->second;
	}
	size_t* skin = new size_t(ds);
	skins[name] = skin;
	CGE::instance->threadPool.runAsync([](size_t* pointer, std::string& name) {
		try {
			std::stringstream ss;
			http_get(std::string("craftgame.alex2772.ru/Craftgame/CraftgameSkins/") + name + ".png", ss);
			Image* i = CGE::instance->imageLoader->loadPNG(ss);
			std::string str = std::string("skin/") + name;
			CGE::instance->uiThread.push(std::function<void()>([pointer, i, str]() {
				*pointer = CGE::instance->textureManager->loadTexture(i, str);
			}));
		}
		catch (HttpException e) {
			if (e.status != 404) {
				CGE::instance->logger->err(std::string("Error loading skin ") + name + ": " + e.what());
			}
		}
		catch ( CraftgameException e) {
			CGE::instance->logger->err(std::string("Error loading skin ") + name + ": " + e.what());
		}
	}, skin, name);
	return skin;
}
#endif