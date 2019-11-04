#pragma once

#define VERSION_CODE 0x0001
#define PROTOCOL_VERSION 1

#ifdef SERVER
#define VERSION "Craftgame Server 0.3.0 INDEV"
#else
#define VERSION "Craftgame 0.3.0 INDEV"
#endif
#define MAJOR_VERSION "0.3"
#ifndef M_PI
#define M_PI 3.141592f
#endif
typedef unsigned char byte;
typedef unsigned short ushort;
