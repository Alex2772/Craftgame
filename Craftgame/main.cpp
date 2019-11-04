#define _CRT_SECURE_NO_WARNINGS
#define GLEW_STATIC
#pragma warning(disable: 4996)

#define EXCEPTION_CATCH

#include "gl.h"
#include "Keyboard.h"
#include "OS.h"

#ifdef WINDOWS
#include <Windows.h>
#include <gl\wglew.h>
#include <MMsystem.h>
#endif

#include <iostream>
#include <sstream>
#include "global.h"
#include "OS.h"
#include <vector>
#include "GameEngine.h"
#include "GuiScreenMessageDialog.h"
#include "easter_messages.h"
#include "CraftgameServer.h"
#include "Utils.h"
#include "Joystick.h"
#include "ConsoleCommandSender.h"

#ifdef SERVER
// Сервер
int main() {
    try {
#ifdef WINDOWS
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
#endif
        CGE::instance->logger->info(VERSION);
        CGE::instance->preInit();
        CGE::instance->postInit();
        CraftgameServer* se;
        CGE::instance->server.setObject(se = new CraftgameServer);
		ConsoleCommandSender ccs;
        std::thread s([&]() {
            while (CGE::instance && se) {
                std::string s;
				std::getline(std::cin, s);
                if (!CGE::instance)
                    return;
               se->dispatchCommand(&ccs, s);
            }
        });
        se->run();
        //CGE::instance->server = nullptr;
        Logger* lg = CGE::instance->logger;
        CGE::instance->logger->info("Shutting down server");

        delete CGE::instance;
		CGE::instance = nullptr;
        delete lg;
		s.join();
    }
    catch ( CraftgameException e) {
        CGE::instance->logger->err(e.what());
    }
    //std::cin.ignore();
    //std::cin.get();
    return 0;
}
#else


#ifdef WINDOWS
#include "resource.h"

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLine(), SW_SHOW);
}
HWND _hwnd;
HDC   _hDC;
HGLRC _hRC;
HGLRC _hRC2;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (CGE::instance)
        switch (uMsg)
        {			
        case WM_SETFOCUS:
			CGE::instance->focusGain();
            break;

        case WM_KILLFOCUS:
			CGE::instance->focusLost();
            break;
        case WM_SIZE:
            if (CGE::instance->glewInitiated)
                CGE::instance->onResize(LOWORD(lParam), HIWORD(lParam));
            break;
		case MM_JOY1BUTTONDOWN:
			CGE::instance->onJoystick();
        	break;
        case WM_LBUTTONDOWN:
            CGE::instance->onClick(0);
            break;
        case WM_RBUTTONDOWN:
            CGE::instance->onClick(1);
            break;
        case WM_MBUTTONDOWN:
            CGE::instance->onClick(2);
            break;
        case WM_MOUSEWHEEL:
            CGE::instance->onWheel((short)HIWORD(wParam) / WHEEL_DELTA);
            break;
        case WM_KEYDOWN:
        {
            byte data = 0;
            data |= !(lParam & 0x40000000) ? 1 : 0;
            CGE::instance->onKeyDown(Keyboard::toKey(wParam), data);
        }
            break;
        case WM_CHAR:
            CGE::instance->onText(wParam);
            break;
        case WM_DESTROY:
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = 640;
            lpMMI->ptMinTrackSize.y = 480;
            break;
        }
        }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::string s = lpCmdLine;
    std::vector<CmdLineArg> args = Parsing::parseArgsFromString(s);
    std::string ip;
	bool fullscreen = false;
    unsigned short port = 24565;
    for (size_t i = 0; i < args.size(); i++) {
        std::string n = args[i].name;
        if (n == "username" &&  !CGE::instance->gameProfile && args[i].type == T_STRING) {
            CGE::instance->gameProfile = new GameProfile(args[i].data);
        }
        else if (n == "server" && args[i].type == T_STRING) {
            ip = args[i].data;
        }
        else if (n == "port" && args[i].type == T_INTEGER) {
            int p = *((int*)args[i].data);
            port = (unsigned short)p;
        }
		else if (n == "fullscreen")
        {
			fullscreen = true;
        }
    }
    srand(std::time(0));

    if (!CGE::instance->gameProfile) {
		ip = "";
    }
#ifdef EXCEPTION_CATCH
	try {
#endif
		setlocale(LC_ALL, "");
		CGE::instance->preInit();

		CGE::instance->logger->info(VERSION);
		if (CGE::instance->gameProfile)
			CGE::instance->logger->info(std::string("Setting up user: ") + CGE::instance->gameProfile->getUsername());

		WNDCLASSEX winClass;
		MSG uMsg;
		memset(&uMsg, 0, sizeof(uMsg));


		winClass.lpszClassName = ("CRAFTGAME");
		winClass.cbSize = sizeof(WNDCLASSEX);
		winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		winClass.lpfnWndProc = WindowProc;
		winClass.hInstance = hInstance;
		winClass.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
		winClass.hIconSm = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
		winClass.hCursor = nullptr;
		winClass.hbrBackground = nullptr;
		winClass.lpszMenuName = nullptr;
		winClass.cbClsExtra = 0;
		winClass.cbWndExtra = 0;
		RegisterClassEx(&winClass);

		std::string title = "Craftgame: ";
		{
			time_t t = std::time(0);
			tm* timeinfo = localtime(&t);

			if (timeinfo->tm_yday == 255)
			{
				title += "programmers' day!";
				goto createWindow;
			}

			for (size_t i = 0; i < sizeof(easter_days) / sizeof(easter_day); i++)
			{
				if (easter_days[i].dayOfMonth == timeinfo->tm_mday && easter_days[i].month == timeinfo->tm_mon + 1)
				{
					title += easter_days[i].name;
					goto createWindow;
				}
			}
		}
		//title += easter_messages[rand() % 16];
		title = "Craftgame";
	createWindow:
		DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        _hwnd = CreateWindowEx(NULL, ("CRAFTGAME"), title.c_str(), style, GetSystemMetrics(SM_CXSCREEN) / 2 - 377, GetSystemMetrics(SM_CYSCREEN) / 2 - 250, 854, 500, nullptr, nullptr, hInstance, nullptr);

		ShowWindow(_hwnd, nCmdShow);
		UpdateWindow(_hwnd);
		if (fullscreen) {
			WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
			DWORD dwStyle = GetWindowLong(_hwnd, GWL_STYLE);
			MONITORINFO mi = { sizeof(mi) };
			if (GetWindowPlacement(_hwnd, &g_wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(_hwnd,
					MONITOR_DEFAULTTOPRIMARY), &mi)) {
				SetWindowLong(_hwnd, GWL_STYLE,
					dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(_hwnd, HWND_TOP,
					mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			}
		}
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        _hDC = GetDC(_hwnd);
        size_t iPixelFormat = ChoosePixelFormat(_hDC, &pfd);
        DescribePixelFormat(_hDC, iPixelFormat, sizeof(pfd), &pfd);
        SetPixelFormat(_hDC, iPixelFormat, &pfd);
        _hRC = wglCreateContext(_hDC);
        _hRC2 = wglCreateContext(_hDC);
        wglMakeCurrent(_hDC, _hRC);

        glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            throw  CraftgameException("glewInit failed");
        }
        CGE::instance->glewInitiated = true;
        const int iPixelFormatAttribList[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0
        };
        int pxf;
        UINT iNumFormats;
        wglChoosePixelFormatARB(_hDC, iPixelFormatAttribList, NULL, 1, &pxf, &iNumFormats);
        SetPixelFormat(_hDC, iPixelFormat, &pfd);
        GLint attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
        wglMakeCurrent(_hDC, 0);
        {
            HGLRC CompHRC = wglCreateContextAttribsARB(_hDC, 0, attribs);
            if (CompHRC && wglMakeCurrent(_hDC, CompHRC)) {
                wglDeleteContext(_hRC);
                _hRC = CompHRC;
            }
            else
                throw CraftgameException("Failed to create OpenGL 3.3 context");
        }
        {
            HGLRC CompHRC = wglCreateContextAttribsARB(_hDC, 0, attribs);
            if (CompHRC && wglMakeCurrent(_hDC, CompHRC)) {
                wglDeleteContext(_hRC2);
                _hRC2 = CompHRC;
            }
            else
                throw CraftgameException("Failed to create OpenGL 3.3 context");
        }

        wglShareLists(_hRC, _hRC2);
        wglMakeCurrent(_hDC, _hRC);

        CGE::instance->logger->fine(std::string("Created OpenGL context ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        bool secondThread = true;
        std::thread t([&]() {
            wglMakeCurrent(_hDC, _hRC2);
            while (secondThread) {
                CGE::instance->onSecondContext();
            }
        });

        ShowCursor(false);
        CGE::instance->init();
        CGE::instance->runOnSecondThread([&]() {
            CGE::instance->postInit();
            if (ip.size()) {
                CGE::instance->threadPool.runAsync([&]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    CGE::instance->connectToServer(ip, port);
                });
            }
        });
        {
            RECT r;
            GetClientRect(_hwnd, &r);
            CGE::instance->onResize(r.right - r.left, r.bottom - r.top);
        }


		Joystick::fetchInfo();
		
        while (uMsg.message != WM_QUIT)
        {
            auto duration = std::chrono::high_resolution_clock::now();
            if (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&uMsg);
                DispatchMessage(&uMsg);
            }
            POINT cursor;
            GetCursorPos(&cursor);
            ScreenToClient(_hwnd, &cursor);
			if (CGE::instance->joystick)
				Joystick::fetchInfo();

            CGE::instance->render(cursor.x, cursor.y);
            SwapBuffers(_hDC);
            CGE::instance->millis = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - duration).count();
        }
        secondThread = false;
        CGE::instance->stcv.notify_one();
        t.join();
		if (CGE::instance->server.isValid()) {
			CGE::instance->server.destruct();
		}
        delete CGE::instance;
        CGE::instance = nullptr;
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(_hRC2);
        wglDeleteContext(_hRC);
        ReleaseDC(_hwnd, _hDC);
        DestroyWindow(_hwnd);
        UnregisterClass(("CRAFTGAME"), hInstance);

#ifdef EXCEPTION_CATCH
    }
    catch ( CraftgameException e) {
        ShowCursor(true);
        MessageBox(_hwnd, (e.what()), ("Error"), MB_ICONERROR | MB_OK);
    }
#endif
    return 0;
}
#else

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "gl.h"
#include <GL/glx.h>

Display* display;
Window win;
Window rootWindow;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

int main(int argc, char* argv[]) {
    display = XOpenDisplay(NULL);
	if (!display) {
		std::cout << ("Failed to open X display\n");
		return 1;
	}
    std::string s;
    for (int i = 0; i < argc; i++) {
	s += std::string(argv[i]) + " ";
    }
    s = s.substr(0, s.length() - 1);
    std::vector<CmdLineArg> args = Parsing::parseArgsFromString(s);
    std::string ip;
    unsigned short port = 24565;
    for (size_t i = 0; i < args.size(); i++) {
        std::string n = args[i].name;
        if (n == "username" && !CGE::instance->gameProfile && args[i].type == T_STRING) {
            CGE::instance->gameProfile = new GameProfile(args[i].data);
        } else if (n == "server" && args[i].type == T_STRING) {
            ip = args[i].data;
        } else if (n == "port" && args[i].type == T_INTEGER) {
            int p = *((int *) args[i].data);
            port = (unsigned short) p;
        }
    }
    srand(std::time(0));
    if (!CGE::instance->gameProfile) {
        WARN("No user specified");
        std::string n = "Player";
        n += std::to_string(rand() % 999 + 1);
        CGE::instance->gameProfile = new GameProfile(n);
    }
#ifdef EXCEPTION_CATCH
    try {
#endif
		CGE::instance->preInit();

        CGE::instance->logger->info(VERSION);
        CGE::instance->logger->info(std::string("Setting up user: ") + CGE::instance->gameProfile->getUsername());

        static int visual_attribs[] =
                {
                        GLX_X_RENDERABLE, True,
                        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                        GLX_RENDER_TYPE, GLX_RGBA_BIT,
                        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
                        GLX_RED_SIZE, 8,
                        GLX_GREEN_SIZE, 8,
                        GLX_BLUE_SIZE, 8,
                        GLX_ALPHA_SIZE, 8,
                        GLX_DEPTH_SIZE, 24,
                        GLX_STENCIL_SIZE, 8,
                        GLX_DOUBLEBUFFER, True,
                        //GLX_SAMPLE_BUFFERS  , 1,
                        //GLX_SAMPLES         , 4,
                        None
                };
        int fbcount;
        GLXFBConfig *fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
        int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

        int i;
        for (i = 0; i < fbcount; ++i) {
            XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
            if (vi) {
                int samp_buf, samples;
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);
                if (best_fbc < 0 || samp_buf && samples > best_num_samp)
                    best_fbc = i, best_num_samp = samples;
                if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
                    worst_fbc = i, worst_num_samp = samples;
            }
            XFree(vi);
        }

        GLXFBConfig bestFbc = fbc[best_fbc];

        // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
        XFree(fbc);

        // Get a visual
        XVisualInfo *vi = glXGetVisualFromFBConfig(display, bestFbc);

        XSetWindowAttributes swa;
        Colormap cmap;
        swa.colormap = cmap = XCreateColormap(display,
                                              RootWindow(display, vi->screen),
                                              vi->visual, AllocNone);
        swa.background_pixmap = None;
        swa.border_pixel = 0;
        swa.event_mask = ButtonPress | KeyPressMask;
		rootWindow = RootWindow(display, vi->screen);

        win = XCreateWindow(display, RootWindow(display, vi->screen),
                                   0, 0, 854, 500, 0, vi->depth, InputOutput,
                                   vi->visual,
                                   CWColormap | CWEventMask, &swa);
        XFree(vi);
		std::string title = "Craftgame: ";
		title += easter_messages[rand() % 16];
        XStoreName(display, win, title.c_str());
        XMapWindow(display, win);

        glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
        glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
                glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");

        GLXContext ctx = 0;
		GLXContext ctx2 = 0;
        int context_attribs[] =
                {
                        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
                        //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                        None
                };
	ctx2 = glXCreateContextAttribsARB(display, bestFbc, 0,
                                         True, context_attribs);
        ctx = glXCreateContextAttribsARB(display, bestFbc, ctx2,
                                         True, context_attribs);

        if (!(display && win && ctx && ctx2)) {
            std::cout << "Error\n";
        }
        glXMakeCurrent(display, win, ctx);
	glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            throw  CraftgameException("glewInit failed");
        }
        CGE::instance->glewInitiated = true;
        __CGE_CE("AFTER GLEW INIT");
	__CGE_CE("CREATING OPENGL CONTEXT");

        CGE::instance->logger->fine(std::string("Created OpenGL context ") + (char*)glGetString(GL_VERSION));
        bool secondThread = true;
        std::thread t([&]() {
            glXMakeCurrent(display, win, ctx2);
            while (secondThread) {
                CGE::instance->onSecondContext();
            }
        });

        //ShowCursor(false);
        CGE::instance->init();
        CGE::instance->runOnSecondThread([&]() {
            CGE::instance->postInit();
            if (ip.size()) {
                CGE::instance->threadPool.runAsync([&]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    CGE::instance->connectToServer(ip, port);
                });
            }
        });

        while (1) {
            XEvent event;
            while (XPending(display)) {
		XNextEvent(display, &event);
		switch(event.type) {
		case ButtonPress: {
		    int button = event.xbutton.button - 1;
		    if (button == 2)
			button = 1;
		    else if (button == 1)
			button = 2;
		    CGE::instance->onClick(button);
		    break;
		}
		case KeyPress:
		    Keyboard::Key key = Keyboard::toKey(event.xkey.keycode);
		    CGE::instance->onKeyDown(key, 0);
			static XComposeStatus status;
			char keyBuffer[16];
			if (key != Keyboard::Escape && key != Keyboard::Return && key != Keyboard::BackSpace && key != Keyboard::Delete && XLookupString(&event.xkey, keyBuffer, sizeof(keyBuffer), NULL, &status))
			{
				char symbol = keyBuffer[0];
				CGE::instance->onText(symbol);
			}
		    break;
		}
            }
	    auto duration = std::chrono::high_resolution_clock::now();
	    int root_x, root_y, win_x, win_y;
	    Window rw, cw;
	    unsigned int mask;
	    XQueryPointer(display, win, &cw,
                &rw, &root_x, &root_y, &win_x, &win_y,
                &mask);
            CGE::instance->render(win_x, win_y);
            CGE::instance->millis = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - duration).count();

            glXSwapBuffers(display, win);
        }
#ifdef EXCEPTION_CATCH
    }
    catch (CraftgameException e) {
        std::cout << e.what() << std::endl;
        //ShowCursor(true);
        //MessageBox(_hwnd, (e.what()), ("Error"), MB_ICONERROR | MB_OK);
    }
#endif

    return 0;
}

#endif
#endif
