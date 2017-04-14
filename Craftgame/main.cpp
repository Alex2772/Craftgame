#define _CRT_SECURE_NO_WARNINGS
#define GLEW_STATIC
#pragma warning(disable: 4996)

#define EXCEPTION_CATCH

#include <GL/glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include "Keyboard.h"
#include "OS.h"
#ifdef WINDOWS
#include <Windows.h>
#endif
#ifdef WINDOWS
#include <gl\wglew.h>
#endif

#include <iostream>
#include "GameEngine.h"
#include <sstream>
#include "global.h"
#include "CraftgameServer.h"
#include "OS.h"
#include <vector>

#ifdef SERVER
int main() {
	try {
#ifdef WINDOWS
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
#endif
		CGE::instance->logger->info(VERSION);
		CGE::instance->preInit();
		CraftgameServer* se;
		CGE::instance->server = se = new CraftgameServer;
		new std::thread([&]() {
			while (CGE::instance->server && se->isRunning) {
				std::string s;
				while (cin.peek() == EOF && se->isRunning)
					OS::sleep(10);
				if (!se->isRunning)
					return;
				getline(cin, s);
				if (!CGE::instance->server)
					return;
				se->dispatchCommand(se->ccs, s);
			}
		});
		se->run();
		CGE::instance->server = nullptr;
		Logger* lg = CGE::instance->logger;
		CGE::instance->logger->info("Shutting down server");
		delete se;
		delete CGE::instance;
		delete lg;
	}
	catch ( CraftgameException e) {
		CGE::instance->logger->err(e.what());
	}
	//std::cin.ignore();
	//std::cin.get();
	return 0;
}
#else
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
	switch (uMsg)
	{
	case WM_SETFOCUS:
		CGE::instance->focus = true;
		break;

	case WM_KILLFOCUS:
		CGE::instance->focus = false;
		break;
	case WM_SIZE:
		if (CGE::instance->glewInitiated)
			CGE::instance->onResize(LOWORD(lParam), HIWORD(lParam));
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
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	std::string s = lpCmdLine;
	std::vector<CmdLineArg> args = Parsing::parseArgsFromString(s);
	std::string ip;
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
		setlocale(LC_ALL, "");
		CGE::instance->preInit();

		CGE::instance->logger->info(VERSION);
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

		_hwnd = CreateWindowEx(NULL, ("CRAFTGAME"), ("Craftgame"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, GetSystemMetrics(SM_CXSCREEN) / 2 - 377, GetSystemMetrics(SM_CYSCREEN) / 2 - 250, 854, 500, nullptr, nullptr, hInstance, nullptr);
		ShowWindow(_hwnd, nCmdShow);
		UpdateWindow(_hwnd);
		

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
		__CGE_CE("AFTER GLEW INIT");
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
		{
			HGLRC CompHRC = wglCreateContextAttribsARB(_hDC, 0, attribs);
			if (CompHRC && wglMakeCurrent(_hDC, CompHRC))
				_hRC = CompHRC;
			else
				throw CraftgameException("Failed to create OpenGL 3.3 context");
		}
		{
			HGLRC CompHRC = wglCreateContextAttribsARB(_hDC, 0, attribs);
			if (CompHRC && wglMakeCurrent(_hDC, CompHRC))
				_hRC2 = CompHRC;
			else
				throw CraftgameException("Failed to create OpenGL 3.3 context");
		}

		wglShareLists(_hRC, _hRC2);
		wglMakeCurrent(_hDC, _hRC);
		__CGE_CE("CREATING OPENGL CONTEXT");

		CGE::instance->logger->fine(std::string("Created OpenGL context ") + (char*)glGetString(GL_VERSION));

		CGE::instance->postInit();
		ShowCursor(false);
		bool secondThread = true;
		std::thread t([&]() {
			wglMakeCurrent(_hDC, _hRC2);
			while (secondThread) {
				CGE::instance->onSecondContext();
			}
		});
	
		if (ip.size()) {
			CGE::instance->threadPool.runAsync([&]() {
				OS::sleep(500);
				CGE::instance->connectToServer(ip, port);
			});
		}
		{
			RECT r;
			GetClientRect(_hwnd, &r);
			CGE::instance->onResize(r.right - r.left, r.bottom - r.top);
		}
		GLenum err;
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
			CGE::instance->render(cursor.x, cursor.y);
			SwapBuffers(_hDC);
			CGE::instance->millis = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - duration).count();
		}
		secondThread = false;
		t.join();
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(_hRC);
		wglDeleteContext(_hRC2);
		_hRC = nullptr;
		ReleaseDC(_hwnd, _hDC);
		_hDC = nullptr;
		DestroyWindow(_hwnd);
		UnregisterClass(("CRAFTGAME"), hInstance);

		delete CGE::instance;
#ifdef EXCEPTION_CATCH
	}
	catch ( CraftgameException e) {
		ShowCursor(true);
		MessageBox(_hwnd, (e.what()), ("Error"), MB_ICONERROR | MB_OK);
	}
#endif
	return 0;
}
#endif
