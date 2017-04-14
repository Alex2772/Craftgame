#include "OS.h"

#ifdef WINDOWS

#include <Windows.h>

void OS::sleep(unsigned long delay) {
	Sleep(delay);
}
#else
#include <unistd.h>
void OS::sleep(unsigned long delay) {
	usleep(delay);
}
#endif
