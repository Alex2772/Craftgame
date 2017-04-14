#pragma once
#ifdef WINDOWS
#include <Windows.h>
#else
typedef uint64_t WPARAM;
#endif

namespace Keyboard {
	enum Key {
		ANY,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		ESCAPE,
		BACKSPACE,
		ENTER,
		ARROW_LEFT,
		ARROW_RIGHT,
		ARROW_UP,
		ARROW_DOWN,
		LCTRL,
		RCTRL,
		LSHIFT,
		DEL,

		LBUTTON,
		RBUTTON,

		A,
		C,
		V,
		X,
	};
	Key toKey(WPARAM w);
	WPARAM fromKey(Key w);
	bool isKeyDown(Key k);
}
