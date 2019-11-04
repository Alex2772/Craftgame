#pragma once
#include "Keyboard.h"
#include <string>
#include <functional>
#include <vector>

class KeyBinding
{
private:
	Keyboard::Key defaultValue;
	Keyboard::Key currentValue;
	std::function<void()> proc;
public:
	std::string name;
	Keyboard::Key getCurrentValue() const
	{
		return currentValue;
	}
	Keyboard::Key getDefaultValue() const
	{
		return defaultValue;
	}
	void setCurrentValue(Keyboard::Key current_value);

	KeyBinding(std::string name, Keyboard::Key defaultValue, std::function<void()> proc);
	std::function<void()> getProc() const
	{
		return proc;
	}
};

class KeyBindingGroup
{
public:
	KeyBindingGroup(std::string n);
	std::vector<KeyBinding*> data;
	std::string name;

};