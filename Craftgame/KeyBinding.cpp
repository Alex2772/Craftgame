#include "KeyBinding.h"
#include "GameEngine.h"

void KeyBinding::setCurrentValue(Keyboard::Key current_value)
{
	currentValue = current_value;
}

KeyBinding::KeyBinding(std::string n, Keyboard::Key d, std::function<void()> _proc):
name(n),
defaultValue(d),
proc(_proc)
{
}

KeyBindingGroup::KeyBindingGroup(std::string n):
name(n)
{
}
