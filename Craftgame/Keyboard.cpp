#ifndef SERVER
#include "Keyboard.h"

using namespace Keyboard;
Key Keyboard::toKey(WPARAM key) {
	switch (key) {
	case VK_ESCAPE:
		return Key::ESCAPE;
	case VK_F1:
		return Key::F1;
	case VK_F2:
		return Key::F2;
	case VK_F3:
		return Key::F3;
	case VK_F4:
		return Key::F4;
	case VK_F5:
		return Key::F5;
	case VK_F6:
		return Key::F6;
	case VK_F7:
		return Key::F7;
	case VK_F8:
		return Key::F8;
	case VK_F9:
		return Key::F9;
	case VK_F10:
		return Key::F10;
	case VK_F11:
		return Key::F11;
	case VK_F12:
		return Key::F12;
	case VK_BACK:
		return Key::BACKSPACE;
	case VK_RETURN:
		return Key::ENTER;
	case VK_UP:
		return Key::ARROW_UP;
	case VK_DOWN:
		return Key::ARROW_DOWN;
	case VK_LEFT:
		return Key::ARROW_LEFT;
	case VK_RIGHT:
		return Key::ARROW_RIGHT;
	case VK_LCONTROL:
		return Key::LCTRL;
	case VK_RCONTROL:
		return Key::RCTRL;
	case VK_LSHIFT:
		return Key::LSHIFT;
	case VK_DELETE:
		return Key::DEL;
	case VK_LBUTTON:
		return Key::LBUTTON;
	case VK_RBUTTON:
		return Key::RBUTTON;
	case 0x41:
		return Key::A;
	case 0x43:
		return Key::C;
	case 0x56:
		return Key::V;
	case 0x58:
		return Key::X;
	default:
		return Key::ANY;
	}
}
WPARAM Keyboard::fromKey(Key key) {
	switch (key) {
	case Key::ESCAPE:
		return VK_ESCAPE;
	case Key::F1:
		return VK_F1;
	case Key::F2:
		return VK_F2;
	case Key::F3:
		return VK_F3;
	case Key::F4:
		return VK_F4;
	case Key::F5:
		return VK_F5;
	case Key::F6:
		return VK_F6;
	case Key::F7:
		return VK_F7;
	case Key::F8:
		return VK_F8;
	case Key::F9:
		return VK_F9;
	case Key::F10:
		return VK_F10;
	case Key::F11:
		return VK_F11;
	case Key::F12:
		return VK_F12;
	case Key::BACKSPACE:
		return VK_BACK;
	case Key::ENTER:
		return VK_RETURN;
	case Key::ARROW_UP:
		return VK_UP;
	case Key::ARROW_DOWN:
		return VK_DOWN;
	case Key::ARROW_LEFT:
		return VK_LEFT;
	case Key::ARROW_RIGHT:
		return VK_RIGHT;
	case Key::LCTRL:
		return VK_LCONTROL;
	case Key::RCTRL:
		return VK_RCONTROL;
	case Key::LSHIFT:
		return VK_LSHIFT;
	case Key::DEL:
		return VK_DELETE;
	case Key::LBUTTON:
		return VK_LBUTTON;
	case Key::RBUTTON:
		return VK_RBUTTON;
	case Key::A:
		return 0x41;
	case Key::X:
		return 0x58;
	case Key::C:
		return 0x43;
	case Key::V:
		return 0x56;
	}
}
bool Keyboard::isKeyDown(Key k) {
	return GetAsyncKeyState(fromKey(k));
}
#endif
