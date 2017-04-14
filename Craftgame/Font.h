#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include <GL\GL.h>
#include "Res.h"
extern "C" {
#include  "ft2build.h"
#include <freetype\freetype.h>
#include <freetype\ftglyph.h>
#include <freetype\ftoutln.h>
#include <freetype\fttrigon.h>
};

#define FONT_SIZE 14

struct Character {
	FT_ULong c;
	unsigned char* data;
	GLuint texture;
	size_t width, height;
	FT_Pos advanceX, advanceY;
};

struct Pair {
	std::string key;
	std::string value;
};

typedef std::vector<std::string> StringArray;
class Font {
private:
	StringArray split(const std::string &s, char delim);
	struct Font_Size {
		long size;
		std::vector<Character*> chars;
	};
	std::vector<Font_Size> data;
	FT_Face face;
	std::string path;
	std::vector<Character*>& getCharsetBySize(long size);
	Character* renderGlyph(FT_ULong glyph, long size);
public:
	_R res;

	Font(std::string path, _R& _res);
	~Font();
	Character* getCharacter(FT_ULong id, long size = FONT_SIZE);
	size_t length(std::string text, long size = FONT_SIZE);
	std::string trimStringToWidth(std::string& text, int width, long size = FONT_SIZE);
	std::vector<std::string> trimStringToMultiline(std::string text, int width, long size = FONT_SIZE);
};