#include "Joystick.h"
#include "GameEngine.h"
#include <Windows.h>
#include <Mmsystem.h>
#include <cmath>


Joystick::Stick Joystick::stick1 = Joystick::Stick();
Joystick::Stick Joystick::stick2 = Joystick::Stick();

std::map<Joystick::Keys, Joystick::JoystickButton> Joystick::_jb = std::map<Joystick::Keys, Joystick::JoystickButton>();

int Joystick::buttons = 0;


void Joystick::fetchInfo()
{
#ifndef SERVER
	JOYINFOEX joyInfo;
	joyInfo.dwSize = sizeof(JOYINFOEX);
	joyInfo.dwFlags = JOY_RETURNALL;
	if (!CGE::instance->focus)
	{
		stick1.x = stick1.y = stick2.x = stick2.y = 0.f;
		buttons = 0;
	} else if (joyGetPosEx(0, &joyInfo) == JOYERR_NOERROR) {
		CGE::instance->joystick = 1;
		stick1.x = roundf((float(joyInfo.dwXpos) / 32768.f - 1) * 100.f) / 100.f;
		stick1.y = roundf((float(joyInfo.dwYpos) / 32768.f - 1) * 100.f) / 100.f;

		stick1.fix();

		stick2.x = roundf((float(joyInfo.dwUpos) / 32768.f - 1) * 100.f) / 100.f;
		stick2.y = roundf((float(joyInfo.dwRpos) / 32768.f - 1) * 100.f) / 100.f;

		stick2.fix();

		buttons = joyInfo.dwButtons;

		if (joyInfo.dwZpos >= 35000) {
			buttons |= L2;
		}
		else if (joyInfo.dwZpos <= 30000) {
			buttons |= R2;
		}
		switch (joyInfo.dwPOV) {
		case 0:
			buttons |= UP;
			break;
		case 9000:
			buttons |= RIGHT;
			break;
		case 18000:
			buttons |= DOWN;
			break;
		case 27000:
			buttons |= LEFT;
			break;
		}
	}
	else {
		CGE::instance->joystick = 0;
	}
#endif
}
void Joystick::input(int key) {

}
void Joystick::setJoystickHandler(Keys k, JoystickButton jb)
{
	if (_jb[k].strength < jb.strength) {
		_jb[k] = jb;
	}
}

int Joystick::iconOf(Keys k) {
	switch (k) {
	case A:
		return 0;
	case B:
		return 1;
	case X:
		return 3;
	case Y:
		return 2;
	case L1:
		return 4;
	case R1:
		return 5;
	case L2:
		return 6;
	case R2:
		return 7;
	case LEFT:
		return 8;
	case RIGHT:
		return 11;
	case UP:
		return 9;
	case DOWN:
		return 10;
	}
	return 0;
}

bool Joystick::getKeyState(Keys key)
{
	return buttons & key;
}
