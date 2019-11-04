#pragma once
#include <map>
#include <string>
#include <functional>

class Joystick {
private:
	static int buttons;

public:
	enum Keys {
		A = 1,
		B = 2,
		X = 4,
		Y = 8,
		L1 = 16,
		R1 = 32,
		BACK = 64,
		START = 128,
		L3 = 256,
		R3 = 512,
		L2 = 1024,
		R2 = 2048,

		UP = 4096,
		LEFT = 8192,
		RIGHT = 16384,
		DOWN = 32768
	};
	struct Stick {
		float x, y;
		void fix() {
			if (abs(x) < 0.1f) {
				x = 0;
			}
			if (abs(y) < 0.1f) {
				y = 0;
			}
		}
	};
	static Stick stick1;
	static Stick stick2;
	static void fetchInfo();
	static bool getKeyState(Keys k);
	static void input(int key);

	class JoystickButton {
	public:
		int strength;
		std::string name;
		std::function<void()> call;
		JoystickButton(int s, std::string n, std::function<void()> c) :
			strength(s), name(n), call(c) {

		}
		JoystickButton() : strength(0) {

		}
	};

	static std::map<Keys, JoystickButton> _jb;
	static void setJoystickHandler(Keys k, JoystickButton jb);
	static int iconOf(Keys k);
};