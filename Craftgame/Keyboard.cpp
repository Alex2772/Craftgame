#ifndef SERVER
//https://github.com/SFML/SFML/blob/master/src/SFML/Window/Win32/InputImpl.cpp
#include "Keyboard.h"
using namespace Keyboard;

#ifdef WINDOWS
#include <Windows.h>
Key Keyboard::toKey(int key) {
	Key vkey;
	switch (key)
	{
	default:           vkey = Keyboard::Unknown;     break;
	case 'A':          vkey = Keyboard::A;           break;
	case 'B':          vkey = Keyboard::B;           break;
	case 'C':          vkey = Keyboard::C;           break;
	case 'D':          vkey = Keyboard::D;           break;
	case 'E':          vkey = Keyboard::E;           break;
	case 'F':          vkey = Keyboard::F;           break;
	case 'G':          vkey = Keyboard::G;           break;
	case 'H':          vkey = Keyboard::H;           break;
	case 'I':          vkey = Keyboard::I;           break;
	case 'J':          vkey = Keyboard::J;           break;
	case 'K':          vkey = Keyboard::K;           break;
	case 'L':          vkey = Keyboard::L;           break;
	case 'M':          vkey = Keyboard::M;           break;
	case 'N':          vkey = Keyboard::N;           break;
	case 'O':          vkey = Keyboard::O;           break;
	case 'P':          vkey = Keyboard::P;           break;
	case 'Q':          vkey = Keyboard::Q;           break;
	case 'R':          vkey = Keyboard::R;           break;
	case 'S':          vkey = Keyboard::S;           break;
	case 'T':          vkey = Keyboard::T;           break;
	case 'U':          vkey = Keyboard::U;           break;
	case 'V':          vkey = Keyboard::V;           break;
	case 'W':          vkey = Keyboard::W;           break;
	case 'X':          vkey = Keyboard::X;           break;
	case 'Y':          vkey = Keyboard::Y;           break;
	case 'Z':          vkey = Keyboard::Z;           break;
	case '0':       vkey = Keyboard::Num0;           break;
	case '1':       vkey = Keyboard::Num1;           break;
	case '2':       vkey = Keyboard::Num2;           break;
	case '3':       vkey = Keyboard::Num3;           break;
	case '4':       vkey = Keyboard::Num4;           break;
	case '5':       vkey = Keyboard::Num5;           break;
	case '6':       vkey = Keyboard::Num6;           break;
	case '7':       vkey = Keyboard::Num7;           break;
	case '8':       vkey = Keyboard::Num8;           break;
	case '9':       vkey = Keyboard::Num9;           break;
	case VK_ESCAPE:     vkey = Keyboard::Escape;     break;
	case VK_CONTROL:
	case VK_LCONTROL:   vkey = Keyboard::LControl;   break;
	case VK_SHIFT:     vkey = Keyboard::LShift;     break;
	case VK_LSHIFT:     vkey = Keyboard::LShift;     break;
	case VK_MENU:       vkey = Keyboard::LAlt;      break;
	case VK_LMENU:       vkey = Keyboard::LAlt;      break;
	case VK_LWIN:    vkey = Keyboard::LSystem;       break;
	case VK_RCONTROL:   vkey = Keyboard::RControl;   break;
	case VK_RSHIFT:     vkey = Keyboard::RShift;     break;
	case VK_RMENU:       vkey = Keyboard::RAlt;      break;
	case VK_RWIN:    vkey = Keyboard::RSystem;       break;
	case VK_APPS:       vkey = Keyboard::Menu;       break;
	case VK_OEM_4:   vkey = Keyboard::LBracket;      break;
	case VK_OEM_6:   vkey = Keyboard::RBracket;      break;
	case VK_OEM_1:  vkey = Keyboard::SemiColon;      break;
	case VK_OEM_COMMA:      vkey = Keyboard::Comma;  break;
	case VK_OEM_PERIOD:     vkey = Keyboard::Period; break;
	case VK_OEM_7:      vkey = Keyboard::Quote;      break;
	case VK_OEM_2:      vkey = Keyboard::Slash;      break;
	case VK_OEM_5:  vkey = Keyboard::BackSlash;      break;
	case VK_OEM_3:      vkey = Keyboard::Tilde;      break;
	case VK_OEM_PLUS:      vkey = Keyboard::Equal;   break;
	case VK_OEM_MINUS:       vkey = Keyboard::Dash;  break;
	case VK_SPACE:      vkey = Keyboard::Space;      break;
	case VK_RETURN:     vkey = Keyboard::Return;     break;
	case VK_BACK:  vkey = Keyboard::BackSpace;       break;
	case VK_TAB:        vkey = Keyboard::Tab;        break;
	case VK_PRIOR:     vkey = Keyboard::PageUp;      break;
	case VK_NEXT:   vkey = Keyboard::PageDown;       break;
	case VK_END:        vkey = Keyboard::End;        break;
	case VK_HOME:       vkey = Keyboard::Home;       break;
	case VK_INSERT:     vkey = Keyboard::Insert;     break;
	case VK_DELETE:     vkey = Keyboard::Delete;     break;
	case VK_ADD:        vkey = Keyboard::Add;        break;
	case VK_SUBTRACT:   vkey = Keyboard::Subtract;   break;
	case VK_MULTIPLY:   vkey = Keyboard::Multiply;   break;
	case VK_DIVIDE:     vkey = Keyboard::Divide;     break;
	case VK_LEFT:       vkey = Keyboard::Left;       break;
	case VK_RIGHT:      vkey = Keyboard::Right;      break;
	case VK_UP:         vkey = Keyboard::Up;         break;
	case VK_DOWN:       vkey = Keyboard::Down;       break;
	case VK_NUMPAD0:    vkey = Keyboard::Numpad0;    break;
	case VK_NUMPAD1:    vkey = Keyboard::Numpad1;    break;
	case VK_NUMPAD2:    vkey = Keyboard::Numpad2;    break;
	case VK_NUMPAD3:    vkey = Keyboard::Numpad3;    break;
	case VK_NUMPAD4:    vkey = Keyboard::Numpad4;    break;
	case VK_NUMPAD5:    vkey = Keyboard::Numpad5;    break;
	case VK_NUMPAD6:    vkey = Keyboard::Numpad6;    break;
	case VK_NUMPAD7:    vkey = Keyboard::Numpad7;    break;
	case VK_NUMPAD8:    vkey = Keyboard::Numpad8;    break;
	case VK_NUMPAD9:    vkey = Keyboard::Numpad9;    break;
	case VK_F1:         vkey = Keyboard::F1;         break;
	case VK_F2:         vkey = Keyboard::F2;         break;
	case VK_F3:         vkey = Keyboard::F3;         break;
	case VK_F4:         vkey = Keyboard::F4;         break;
	case VK_F5:         vkey = Keyboard::F5;         break;
	case VK_F6:         vkey = Keyboard::F6;         break;
	case VK_F7:         vkey = Keyboard::F7;         break;
	case VK_F8:         vkey = Keyboard::F8;         break;
	case VK_F9:         vkey = Keyboard::F9;         break;
	case VK_F10:        vkey = Keyboard::F10;        break;
	case VK_F11:        vkey = Keyboard::F11;        break;
	case VK_F12:        vkey = Keyboard::F12;        break;
	case VK_F13:        vkey = Keyboard::F13;        break;
	case VK_F14:        vkey = Keyboard::F14;        break;
	case VK_F15:        vkey = Keyboard::F15;        break;
	case VK_PAUSE:      vkey = Keyboard::Pause;      break;
	case VK_LBUTTON:    vkey = Keyboard::LButton;    break;
	case VK_RBUTTON:    vkey = Keyboard::RButton;    break;
	}
	return vkey;
}
int Keyboard::fromKey(Key key) {
	int vkey = 0;
	switch (key)
	{
	default:                   vkey = 0;             break;
	case Keyboard::A:          vkey = 'A';           break;
	case Keyboard::B:          vkey = 'B';           break;
	case Keyboard::C:          vkey = 'C';           break;
	case Keyboard::D:          vkey = 'D';           break;
	case Keyboard::E:          vkey = 'E';           break;
	case Keyboard::F:          vkey = 'F';           break;
	case Keyboard::G:          vkey = 'G';           break;
	case Keyboard::H:          vkey = 'H';           break;
	case Keyboard::I:          vkey = 'I';           break;
	case Keyboard::J:          vkey = 'J';           break;
	case Keyboard::K:          vkey = 'K';           break;
	case Keyboard::L:          vkey = 'L';           break;
	case Keyboard::M:          vkey = 'M';           break;
	case Keyboard::N:          vkey = 'N';           break;
	case Keyboard::O:          vkey = 'O';           break;
	case Keyboard::P:          vkey = 'P';           break;
	case Keyboard::Q:          vkey = 'Q';           break;
	case Keyboard::R:          vkey = 'R';           break;
	case Keyboard::S:          vkey = 'S';           break;
	case Keyboard::T:          vkey = 'T';           break;
	case Keyboard::U:          vkey = 'U';           break;
	case Keyboard::V:          vkey = 'V';           break;
	case Keyboard::W:          vkey = 'W';           break;
	case Keyboard::X:          vkey = 'X';           break;
	case Keyboard::Y:          vkey = 'Y';           break;
	case Keyboard::Z:          vkey = 'Z';           break;
	case Keyboard::Num0:       vkey = '0';           break;
	case Keyboard::Num1:       vkey = '1';           break;
	case Keyboard::Num2:       vkey = '2';           break;
	case Keyboard::Num3:       vkey = '3';           break;
	case Keyboard::Num4:       vkey = '4';           break;
	case Keyboard::Num5:       vkey = '5';           break;
	case Keyboard::Num6:       vkey = '6';           break;
	case Keyboard::Num7:       vkey = '7';           break;
	case Keyboard::Num8:       vkey = '8';           break;
	case Keyboard::Num9:       vkey = '9';           break;
	case Keyboard::Escape:     vkey = VK_ESCAPE;     break;
	case Keyboard::LControl:   vkey = VK_LCONTROL;   break;
	case Keyboard::LShift:     vkey = VK_LSHIFT;     break;
	case Keyboard::LAlt:       vkey = VK_LMENU;      break;
	case Keyboard::LSystem:    vkey = VK_LWIN;       break;
	case Keyboard::RControl:   vkey = VK_RCONTROL;   break;
	case Keyboard::RShift:     vkey = VK_RSHIFT;     break;
	case Keyboard::RAlt:       vkey = VK_RMENU;      break;
	case Keyboard::RSystem:    vkey = VK_RWIN;       break;
	case Keyboard::Menu:       vkey = VK_APPS;       break;
	case Keyboard::LBracket:   vkey = VK_OEM_4;      break;
	case Keyboard::RBracket:   vkey = VK_OEM_6;      break;
	case Keyboard::SemiColon:  vkey = VK_OEM_1;      break;
	case Keyboard::Comma:      vkey = VK_OEM_COMMA;  break;
	case Keyboard::Period:     vkey = VK_OEM_PERIOD; break;
	case Keyboard::Quote:      vkey = VK_OEM_7;      break;
	case Keyboard::Slash:      vkey = VK_OEM_2;      break;
	case Keyboard::BackSlash:  vkey = VK_OEM_5;      break;
	case Keyboard::Tilde:      vkey = VK_OEM_3;      break;
	case Keyboard::Equal:      vkey = VK_OEM_PLUS;   break;
	case Keyboard::Dash:       vkey = VK_OEM_MINUS;  break;
	case Keyboard::Space:      vkey = VK_SPACE;      break;
	case Keyboard::Return:     vkey = VK_RETURN;     break;
	case Keyboard::BackSpace:  vkey = VK_BACK;       break;
	case Keyboard::Tab:        vkey = VK_TAB;        break;
	case Keyboard::PageUp:     vkey = VK_PRIOR;      break;
	case Keyboard::PageDown:   vkey = VK_NEXT;       break;
	case Keyboard::End:        vkey = VK_END;        break;
	case Keyboard::Home:       vkey = VK_HOME;       break;
	case Keyboard::Insert:     vkey = VK_INSERT;     break;
	case Keyboard::Delete:     vkey = VK_DELETE;     break;
	case Keyboard::Add:        vkey = VK_ADD;        break;
	case Keyboard::Subtract:   vkey = VK_SUBTRACT;   break;
	case Keyboard::Multiply:   vkey = VK_MULTIPLY;   break;
	case Keyboard::Divide:     vkey = VK_DIVIDE;     break;
	case Keyboard::Left:       vkey = VK_LEFT;       break;
	case Keyboard::Right:      vkey = VK_RIGHT;      break;
	case Keyboard::Up:         vkey = VK_UP;         break;
	case Keyboard::Down:       vkey = VK_DOWN;       break;
	case Keyboard::Numpad0:    vkey = VK_NUMPAD0;    break;
	case Keyboard::Numpad1:    vkey = VK_NUMPAD1;    break;
	case Keyboard::Numpad2:    vkey = VK_NUMPAD2;    break;
	case Keyboard::Numpad3:    vkey = VK_NUMPAD3;    break;
	case Keyboard::Numpad4:    vkey = VK_NUMPAD4;    break;
	case Keyboard::Numpad5:    vkey = VK_NUMPAD5;    break;
	case Keyboard::Numpad6:    vkey = VK_NUMPAD6;    break;
	case Keyboard::Numpad7:    vkey = VK_NUMPAD7;    break;
	case Keyboard::Numpad8:    vkey = VK_NUMPAD8;    break;
	case Keyboard::Numpad9:    vkey = VK_NUMPAD9;    break;
	case Keyboard::F1:         vkey = VK_F1;         break;
	case Keyboard::F2:         vkey = VK_F2;         break;
	case Keyboard::F3:         vkey = VK_F3;         break;
	case Keyboard::F4:         vkey = VK_F4;         break;
	case Keyboard::F5:         vkey = VK_F5;         break;
	case Keyboard::F6:         vkey = VK_F6;         break;
	case Keyboard::F7:         vkey = VK_F7;         break;
	case Keyboard::F8:         vkey = VK_F8;         break;
	case Keyboard::F9:         vkey = VK_F9;         break;
	case Keyboard::F10:        vkey = VK_F10;        break;
	case Keyboard::F11:        vkey = VK_F11;        break;
	case Keyboard::F12:        vkey = VK_F12;        break;
	case Keyboard::F13:        vkey = VK_F13;        break;
	case Keyboard::F14:        vkey = VK_F14;        break;
	case Keyboard::F15:        vkey = VK_F15;        break;
	case Keyboard::Pause:      vkey = VK_PAUSE;      break;
	case Keyboard::LButton:    vkey = VK_LBUTTON;    break;
	case Keyboard::RButton:    vkey = VK_RBUTTON;    break;
	}
	return vkey;
}

std::string Keyboard::getName(Key k)
{
	char buffer[16];
	GetKeyNameText(MapVirtualKey(fromKey(k), MAPVK_VK_TO_VSC) << 16, buffer, sizeof(buffer));
	return std::string(buffer);
}

bool Keyboard::isKeyDown(Key k) {
	return GetAsyncKeyState(fromKey(k)) & 32768;
}
#else
#include <X11/Xlib.h>
#include <X11/keysym.h>
extern Display* display;
bool isMouseKeyDown(Keyboard::Key button) {
    // we don't care about these but they are required
    ::Window root, child;
    int wx, wy;
    int gx, gy;

    unsigned int buttons = 0;
    XQueryPointer(display, DefaultRootWindow(display), &root, &child, &gx, &gy, &wx, &wy, &buttons);

    switch (button)
    {
        case Key::LButton:     return buttons & Button1Mask;
        case Key::RButton:    return buttons & Button3Mask;
        //scase Key::MButton:   return buttons & Button2Mask;
        default:              return false;
    }

    return false;
}
Key Keyboard::toKey(int k) {
	Key key;
    KeySym keycode = XKeycodeToKeysym(display, k, 0);
	switch (keycode) {
		case XK_Shift_L: key = Keyboard::LShift; break;
		case XK_Shift_R: key = Keyboard::RShift; break;
		case XK_Control_L: key = Keyboard::LControl; break;
		case XK_Control_R: key = Keyboard::RControl; break;
		case XK_Alt_L: key = Keyboard::LAlt; break;
		case XK_Alt_R: key = Keyboard::RAlt; break;
		case XK_Super_L: key = Keyboard::LSystem; break;
		case XK_Super_R: key = Keyboard::RSystem; break;
		case XK_Menu: key = Keyboard::Menu; break;
		case XK_Escape: key = Keyboard::Escape; break;
		case XK_semicolon: key = Keyboard::SemiColon; break;
		case XK_slash: key = Keyboard::Slash; break;
		case XK_equal: key = Keyboard::Equal; break;
		case XK_minus: key = Keyboard::Dash; break;
		case XK_bracketleft: key = Keyboard::LBracket; break;
		case XK_bracketright: key = Keyboard::RBracket; break;
		case XK_comma: key = Keyboard::Comma; break;
		case XK_period: key = Keyboard::Period; break;
		case XK_apostrophe: key = Keyboard::Quote; break;
		case XK_backslash: key = Keyboard::BackSlash; break;
		case XK_grave: key = Keyboard::Tilde; break;
		case XK_space: key = Keyboard::Space; break;
		case XK_Return: key = Keyboard::Return; break;
		case XK_BackSpace: key = Keyboard::BackSpace; break;
		case XK_Tab: key = Keyboard::Tab; break;
		case XK_Prior: key = Keyboard::PageUp; break;
		case XK_Next: key = Keyboard::PageDown; break;
		case XK_End: key = Keyboard::End; break;
		case XK_Home: key = Keyboard::Home; break;
		case XK_Insert: key = Keyboard::Insert; break;
		case XK_Delete: key = Keyboard::Delete; break;
		case XK_KP_Add: key = Keyboard::Add; break;
		case XK_KP_Subtract: key = Keyboard::Subtract; break;
		case XK_KP_Multiply: key = Keyboard::Multiply; break;
		case XK_KP_Divide: key = Keyboard::Divide; break;
		case XK_Pause: key = Keyboard::Pause; break;
		case XK_F1: key = Keyboard::F1; break;
		case XK_F2: key = Keyboard::F2; break;
		case XK_F3: key = Keyboard::F3; break;
		case XK_F4: key = Keyboard::F4; break;
		case XK_F5: key = Keyboard::F5; break;
		case XK_F6: key = Keyboard::F6; break;
		case XK_F7: key = Keyboard::F7; break;
		case XK_F8: key = Keyboard::F8; break;
		case XK_F9: key = Keyboard::F9; break;
		case XK_F10: key = Keyboard::F10; break;
		case XK_F11: key = Keyboard::F11; break;
		case XK_F12: key = Keyboard::F12; break;
		case XK_F13: key = Keyboard::F13; break;
		case XK_F14: key = Keyboard::F14; break;
		case XK_F15: key = Keyboard::F15; break;
		case XK_Left: key = Keyboard::Left; break;
		case XK_Right: key = Keyboard::Right; break;
		case XK_Up: key = Keyboard::Up; break;
		case XK_Down: key = Keyboard::Down; break;
		case XK_KP_Insert: key = Keyboard::Numpad0; break;
		case XK_KP_End: key = Keyboard::Numpad1; break;
		case XK_KP_Down: key = Keyboard::Numpad2; break;
		case XK_KP_Page_Down: key = Keyboard::Numpad3; break;
		case XK_KP_Left: key = Keyboard::Numpad4; break;
		case XK_KP_Begin: key = Keyboard::Numpad5; break;
		case XK_KP_Right: key = Keyboard::Numpad6; break;
		case XK_KP_Home: key = Keyboard::Numpad7; break;
		case XK_KP_Up: key = Keyboard::Numpad8; break;
		case XK_KP_Page_Up: key = Keyboard::Numpad9; break;
		case XK_a: key = Keyboard::A; break;
		case XK_b: key = Keyboard::B; break;
		case XK_c: key = Keyboard::C; break;
		case XK_d: key = Keyboard::D; break;
		case XK_e: key = Keyboard::E; break;
		case XK_f: key = Keyboard::F; break;
		case XK_g: key = Keyboard::G; break;
		case XK_h: key = Keyboard::H; break;
		case XK_i: key = Keyboard::I; break;
		case XK_j: key = Keyboard::J; break;
		case XK_k: key = Keyboard::K; break;
		case XK_l: key = Keyboard::L; break;
		case XK_m: key = Keyboard::M; break;
		case XK_n: key = Keyboard::N; break;
		case XK_o: key = Keyboard::O; break;
		case XK_p: key = Keyboard::P; break;
		case XK_q: key = Keyboard::Q; break;
		case XK_r: key = Keyboard::R; break;
		case XK_s: key = Keyboard::S; break;
		case XK_t: key = Keyboard::T; break;
		case XK_u: key = Keyboard::U; break;
		case XK_v: key = Keyboard::V; break;
		case XK_w: key = Keyboard::W; break;
		case XK_x: key = Keyboard::X; break;
		case XK_y: key = Keyboard::Y; break;
		case XK_z: key = Keyboard::Z; break;
		case XK_0: key = Keyboard::Num0; break;
		case XK_1: key = Keyboard::Num1; break;
		case XK_2: key = Keyboard::Num2; break;
		case XK_3: key = Keyboard::Num3; break;
		case XK_4: key = Keyboard::Num4; break;
		case XK_5: key = Keyboard::Num5; break;
		case XK_6: key = Keyboard::Num6; break;
		case XK_7: key = Keyboard::Num7; break;
		case XK_8: key = Keyboard::Num8; break;
		case XK_9: key = Keyboard::Num9; break;
		default: key = Keyboard::Unknown; break;
	}
	return key;
}
int Keyboard::fromKey(Key key) {
    int keysym = 0;
    switch (key)
    {
        case Keyboard::LShift:     keysym = XK_Shift_L;      break;
        case Keyboard::RShift:     keysym = XK_Shift_R;      break;
        case Keyboard::LControl:   keysym = XK_Control_L;    break;
        case Keyboard::RControl:   keysym = XK_Control_R;    break;
        case Keyboard::LAlt:       keysym = XK_Alt_L;        break;
        case Keyboard::RAlt:       keysym = XK_Alt_R;        break;
        case Keyboard::LSystem:    keysym = XK_Super_L;      break;
        case Keyboard::RSystem:    keysym = XK_Super_R;      break;
        case Keyboard::Menu:       keysym = XK_Menu;         break;
        case Keyboard::Escape:     keysym = XK_Escape;       break;
        case Keyboard::SemiColon:  keysym = XK_semicolon;    break;
        case Keyboard::Slash:      keysym = XK_slash;        break;
        case Keyboard::Equal:      keysym = XK_equal;        break;
        case Keyboard::Dash:       keysym = XK_minus;        break;
        case Keyboard::LBracket:   keysym = XK_bracketleft;  break;
        case Keyboard::RBracket:   keysym = XK_bracketright; break;
        case Keyboard::Comma:      keysym = XK_comma;        break;
        case Keyboard::Period:     keysym = XK_period;       break;
        case Keyboard::Quote:      keysym = XK_apostrophe;   break;
        case Keyboard::BackSlash:  keysym = XK_backslash;    break;
        case Keyboard::Tilde:      keysym = XK_grave;        break;
        case Keyboard::Space:      keysym = XK_space;        break;
        case Keyboard::Return:     keysym = XK_Return;       break;
        case Keyboard::BackSpace:  keysym = XK_BackSpace;    break;
        case Keyboard::Tab:        keysym = XK_Tab;          break;
        case Keyboard::PageUp:     keysym = XK_Prior;        break;
        case Keyboard::PageDown:   keysym = XK_Next;         break;
        case Keyboard::End:        keysym = XK_End;          break;
        case Keyboard::Home:       keysym = XK_Home;         break;
        case Keyboard::Insert:     keysym = XK_Insert;       break;
        case Keyboard::Delete:     keysym = XK_Delete;       break;
        case Keyboard::Add:        keysym = XK_KP_Add;       break;
        case Keyboard::Subtract:   keysym = XK_KP_Subtract;  break;
        case Keyboard::Multiply:   keysym = XK_KP_Multiply;  break;
        case Keyboard::Divide:     keysym = XK_KP_Divide;    break;
        case Keyboard::Pause:      keysym = XK_Pause;        break;
        case Keyboard::F1:         keysym = XK_F1;           break;
        case Keyboard::F2:         keysym = XK_F2;           break;
        case Keyboard::F3:         keysym = XK_F3;           break;
        case Keyboard::F4:         keysym = XK_F4;           break;
        case Keyboard::F5:         keysym = XK_F5;           break;
        case Keyboard::F6:         keysym = XK_F6;           break;
        case Keyboard::F7:         keysym = XK_F7;           break;
        case Keyboard::F8:         keysym = XK_F8;           break;
        case Keyboard::F9:         keysym = XK_F9;           break;
        case Keyboard::F10:        keysym = XK_F10;          break;
        case Keyboard::F11:        keysym = XK_F11;          break;
        case Keyboard::F12:        keysym = XK_F12;          break;
        case Keyboard::F13:        keysym = XK_F13;          break;
        case Keyboard::F14:        keysym = XK_F14;          break;
        case Keyboard::F15:        keysym = XK_F15;          break;
        case Keyboard::Left:       keysym = XK_Left;         break;
        case Keyboard::Right:      keysym = XK_Right;        break;
        case Keyboard::Up:         keysym = XK_Up;           break;
        case Keyboard::Down:       keysym = XK_Down;         break;
        case Keyboard::Numpad0:    keysym = XK_KP_Insert;    break;
        case Keyboard::Numpad1:    keysym = XK_KP_End;       break;
        case Keyboard::Numpad2:    keysym = XK_KP_Down;      break;
        case Keyboard::Numpad3:    keysym = XK_KP_Page_Down; break;
        case Keyboard::Numpad4:    keysym = XK_KP_Left;      break;
        case Keyboard::Numpad5:    keysym = XK_KP_Begin;     break;
        case Keyboard::Numpad6:    keysym = XK_KP_Right;     break;
        case Keyboard::Numpad7:    keysym = XK_KP_Home;      break;
        case Keyboard::Numpad8:    keysym = XK_KP_Up;        break;
        case Keyboard::Numpad9:    keysym = XK_KP_Page_Up;   break;
        case Keyboard::A:          keysym = XK_a;            break;
        case Keyboard::B:          keysym = XK_b;            break;
        case Keyboard::C:          keysym = XK_c;            break;
        case Keyboard::D:          keysym = XK_d;            break;
        case Keyboard::E:          keysym = XK_e;            break;
        case Keyboard::F:          keysym = XK_f;            break;
        case Keyboard::G:          keysym = XK_g;            break;
        case Keyboard::H:          keysym = XK_h;            break;
        case Keyboard::I:          keysym = XK_i;            break;
        case Keyboard::J:          keysym = XK_j;            break;
        case Keyboard::K:          keysym = XK_k;            break;
        case Keyboard::L:          keysym = XK_l;            break;
        case Keyboard::M:          keysym = XK_m;            break;
        case Keyboard::N:          keysym = XK_n;            break;
        case Keyboard::O:          keysym = XK_o;            break;
        case Keyboard::P:          keysym = XK_p;            break;
        case Keyboard::Q:          keysym = XK_q;            break;
        case Keyboard::R:          keysym = XK_r;            break;
        case Keyboard::S:          keysym = XK_s;            break;
        case Keyboard::T:          keysym = XK_t;            break;
        case Keyboard::U:          keysym = XK_u;            break;
        case Keyboard::V:          keysym = XK_v;            break;
        case Keyboard::W:          keysym = XK_w;            break;
        case Keyboard::X:          keysym = XK_x;            break;
        case Keyboard::Y:          keysym = XK_y;            break;
        case Keyboard::Z:          keysym = XK_z;            break;
        case Keyboard::Num0:       keysym = XK_0;            break;
        case Keyboard::Num1:       keysym = XK_1;            break;
        case Keyboard::Num2:       keysym = XK_2;            break;
        case Keyboard::Num3:       keysym = XK_3;            break;
        case Keyboard::Num4:       keysym = XK_4;            break;
        case Keyboard::Num5:       keysym = XK_5;            break;
        case Keyboard::Num6:       keysym = XK_6;            break;
        case Keyboard::Num7:       keysym = XK_7;            break;
        case Keyboard::Num8:       keysym = XK_8;            break;
        case Keyboard::Num9:       keysym = XK_9;            break;
        default:                   keysym = 0;               break;
    }
    return keysym;
}
bool Keyboard::isKeyDown(Key k) {
	if (k == Key::LButton || k == Key::RButton) {
		return isMouseKeyDown(k);
	}
    KeySym keysym = (KeySym)fromKey(k);

    // Convert to keycode
    KeyCode keycode = XKeysymToKeycode(display, keysym);
    if (keycode != 0)
    {
        // Get the whole keyboard state
        char keys[32];
        XQueryKeymap(display, keys);
		
        // Check our keycode
        return (keys[keycode / 8] & (1 << (keycode % 8))) != 0;
    }
    else
    {
        return false;
    }
}
std::string Keyboard::getName(Key k)
{
        return std::string("?");
}
#endif
#endif
