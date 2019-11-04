#include "OS.h"
#include "GameEngine.h"
#include <gl/wglew.h>

#ifdef WINDOWS


#include <Windows.h>
#include <psapi.h>
#ifndef SERVER
extern HWND _hwnd;
#endif

void OS::setCursorPos(int x, int y)
{
#ifndef SERVER
	POINT l = { x, y };
	ClientToScreen(_hwnd, &l);
	SetCursorPos(l.x, l.y);
	//CGE::instance->mouseX = x;
	//CGE::instance->mouseY = y;
#endif
}
std::string OS::locale()
{
	char szISOLang[5] = { 0 };
	char szISOCountry[5] = { 0 };
	int iLen = 0;

	::GetLocaleInfo(LOCALE_USER_DEFAULT,
		LOCALE_SISO639LANGNAME,
		szISOLang,
		sizeof(szISOLang));

	::GetLocaleInfo(LOCALE_USER_DEFAULT,
		LOCALE_SISO3166CTRYNAME,
		szISOCountry,
		sizeof(szISOCountry));
	return std::string(szISOLang) + "_" + std::string(szISOCountry);
}

void OS::getCursorPos(int& x, int& y)
{
	POINT cursor;
	GetCursorPos(&cursor);
#ifndef SERVER
	ScreenToClient(_hwnd, &cursor);
#endif
	x = cursor.x;
	y = cursor.y;
}
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
extern Display* display;	
extern Window win;	
extern Window rootWindow;	

void OS::setCursorPos(int x, int y)
{
	XWarpPointer(display, win, win, 0, 0, 0, 0, x, y);
    XFlush(display);
	CGE::instance->mouseX = x;
	CGE::instance->mouseY = y;
}
void OS::locale() {
	return "en_US";
}
#endif
