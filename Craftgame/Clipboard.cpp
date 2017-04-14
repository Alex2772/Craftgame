#ifndef SERVER
#include "Clipboard.h"
#include "global.h"
#include <memory>

#ifdef WINDOWS

#include <Windows.h>

void Clipboard::copyToClipboard(const char* str) {
	size_t len = strlen(str) + 1;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hGlobal), str, len);
	GlobalUnlock(hGlobal);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
}
char* Clipboard::copyFromClipboard() {
	if (OpenClipboard(nullptr)) {
		HANDLE clip = GetClipboardData(CF_TEXT);
		if (clip) {
			char* c = reinterpret_cast<char*>(GlobalLock(clip));
			size_t len = strlen(c) + 1;
			char* ch = (char*)malloc(len);
			memcpy(ch, c, len);
			GlobalUnlock(clip);
			return ch;
		}
	}
	return nullptr;
}
#endif
#endif