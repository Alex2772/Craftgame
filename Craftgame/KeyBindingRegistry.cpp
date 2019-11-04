#ifndef SERVER
#include "KeyBindingRegistry.h"

KeyBindingRegistry::~KeyBindingRegistry()
{
	for (size_t i = 0; i < groupData.size(); i++)
	{
		for (size_t j = 0; j < groupData[i]->data.size(); j++)
		{
			delete groupData[i]->data[j];
		}
		delete groupData[i];
	}
}

void KeyBindingRegistry::registerKeyBinding(KeyBindingGroup* g)
{
	groupData.push_back(g);
	for (size_t i = 0; i < g->data.size(); i++)
	{
		_data.push_back(g->data[i]);
	}
}

KeyBinding* KeyBindingRegistry::fromKey(Keyboard::Key k)
{
	for (size_t i = 0; i < _data.size(); i++)
	{
		if (_data[i]->getCurrentValue() == k)
		{
			return _data[i];
		}
	}
	return nullptr;
}

std::vector<KeyBindingGroup*>& KeyBindingRegistry::getGroupData()
{
	return groupData;
}

std::vector<KeyBinding*>& KeyBindingRegistry::getData()
{
	return _data;
}

void KeyBindingRegistry::load(Config::ConfigFile* settings)
{
	for (size_t i = 0; i < groupData.size(); i++)
	{
		std::string s = std::string("craftgame:input/") + groupData[i]->name + "/";
		for (size_t j = 0; j < groupData[i]->data.size(); j++)
		{
			KeyBinding* kb = groupData[i]->data[j];
			_R r = _R(s + kb->name);
			settings->bindDefaultValue(r, Keyboard::fromKey(kb->getDefaultValue()));
			kb->setCurrentValue(Keyboard::toKey(settings->getProperty<int>(r)));
		}
	}
}
void KeyBindingRegistry::save(Config::ConfigFile* settings)
{
	for (size_t i = 0; i < groupData.size(); i++)
	{
		std::string s = std::string("craftgame:input/") + groupData[i]->name + "/";
		for (size_t j = 0; j < groupData[i]->data.size(); j++)
		{
			KeyBinding* kb = groupData[i]->data[j];
			_R r = _R(s + kb->name);
			int k = Keyboard::fromKey(kb->getCurrentValue());
			if (settings->getProperty<int>(r) != k)
				settings->setProperty(r, k);
		}
	}
	settings->apply();
}
#endif