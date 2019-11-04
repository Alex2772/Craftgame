#pragma once
#ifndef SERVER
#include "Keyboard.h"
#include "KeyBinding.h"
#include <map>
#include <vector>
#include "Config.h"

class KeyBindingRegistry
{
	std::vector<KeyBinding*> _data;
	std::vector<KeyBindingGroup*> groupData;
public:
	KeyBindingRegistry()
	{
	}
	virtual ~KeyBindingRegistry();

	void registerKeyBinding(KeyBindingGroup* g);
	KeyBinding* fromKey(Keyboard::Key k);
	std::vector<KeyBindingGroup*>& getGroupData();
	std::vector<KeyBinding*>& getData();
	void load(Config::ConfigFile* settings);
	void save(Config::ConfigFile* settings);
};
#endif