#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "TexturePacker.h"
#include "gl.h"
#include "Res.h"
#include "GLTexture.h"
#include <boost/smart_ptr/make_shared.hpp>

extern "C" {
#include  <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
};

#define FONT_SIZE 14

struct Character {
	FT_ULong c;
	size_t width, height;
	FT_Pos advanceX, advanceY;
	boost::shared_ptr<glm::vec4> uv;
};

struct Pair {
	std::string key;
	std::string value;
};

typedef std::vector<std::string> StringArray;
class CFont {
private:
	struct CFont_Size {
		long size;
		std::vector<Character*> chars;
		boost::shared_ptr<TexturePacker> tp = boost::make_shared<TexturePacker>(1);
		boost::shared_ptr<GLTexture> texture;
		bool isDirty = true;
	};
	std::vector<CFont_Size> data;
	FT_Face face;
	std::string path;
	CFont_Size& getCharsetBySize(long size);
	Character* renderGlyph(CFont_Size& fs, FT_ULong glyph, long size);
public:
	_R res;

	CFont(std::string path, _R _res);
	~CFont();
	Character* getCharacter(FT_ULong id, long size = FONT_SIZE);
	size_t length(std::string text, long size = FONT_SIZE);
	boost::shared_ptr<GLTexture> textureOf(long size);
	std::string trimStringToWidth(std::string& text, int width, long size = FONT_SIZE);
	std::vector<std::string> trimStringToMultiline(std::string text, int width, long size = FONT_SIZE);
};
