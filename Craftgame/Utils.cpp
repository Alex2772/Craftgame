#include "Utils.h"
#include "global.h"


glm::vec4 Utils::fromHex(size_t color) {
	byte r = color >> 24;
	byte g = color >> 16;
	byte b = color >> 8;
	byte a = color;
	return glm::vec4(float(r) / 255.f, float(g) / 255.f, float(b) / 255.f, float(a) / 255.f);
}