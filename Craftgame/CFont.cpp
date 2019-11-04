#ifndef SERVER
#pragma warning(disable: 4996)
#include "CFont.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include "GameEngine.h"


CFont::CFont(std::string _path, _R _res) :
	res(_res),
	path(_path)
{
	if (FT_New_Face(CGE::instance->ft, _path.c_str(), 0, &face)) {
		throw std::runtime_error(std::string("Failed to load font ") + _path + ": FT_Face could not be created");
	}
}
CFont::~CFont() {
	FT_Done_Face(face);
	for (size_t i = 0; i < data.size(); i++) {
		std::vector<Character*> c = data[i].chars;
		for (size_t j = 0; j < c.size(); j++) {
			Character* ch = c[j];
			if (ch) {
				delete ch;
			}
		}
		c.clear();
	}
	data.clear();
}
CFont::CFont_Size& CFont::getCharsetBySize(long size) {
	for (size_t i = 0; i < data.size(); i++) {
		if (data[i].size == size)
			return data[i];
	}
	CFont_Size s;
	s.size = size;
	data.push_back(s);
	//renderGlyphs(size);
	return data[data.size() - 1];
}
Character* CFont::renderGlyph(CFont_Size& fs, FT_ULong glyph, long size) {
	char c = glyph;
	std::wstring wc(sizeof(c), L'#');
	mbstowcs(&wc[0], (char*)&c, sizeof(c));
	FT_Set_Pixel_Sizes(face, 0, size);
	if (FT_Load_Char(face, wc[0], FT_LOAD_RENDER)) {
		throw std::runtime_error(std::string("Cannot load char ") + std::to_string(glyph));
	}
	FT_GlyphSlot g = face->glyph;
	if (g->bitmap.width && g->bitmap.rows) {
		if (glyph == 'w')
		{
			std::cout << "Lol";
		}
		Character* c = new Character;
		c->c = glyph;
		c->width = g->bitmap.width;
		c->height = g->bitmap.rows;
		c->advanceX = g->advance.x >> 6;
		c->advanceY = -(g->metrics.horiBearingY >> 6) + size;
		Image img = 1;
		img.width = c->width;
		img.height = c->height;
		img.data = new unsigned char[g->bitmap.width * g->bitmap.rows];
		memcpy(img.data, g->bitmap.buffer, c->width * c->height);
		c->uv = fs.tp->insert(&img);
		fs.isDirty = true;
		return c;
	}
	return nullptr;
}

boost::shared_ptr<GLTexture> CFont::textureOf(long size) {
	CFont_Size& chars = getCharsetBySize(size);
	if (!chars.texture)
	{
		chars.texture = boost::make_shared<GLTexture>();
	}
	if (chars.isDirty)
	{
		chars.isDirty = false;
		chars.texture->texImage2D(chars.tp->mImage);
	}
	return chars.texture;
}
Character* CFont::getCharacter(FT_ULong id, long size) {
	CFont_Size& chars = getCharsetBySize(size);
	if (chars.chars.size() > id && chars.chars[id]) {
		return chars.chars[id];
	}
	else {
		if (chars.chars.size() <= id)
			chars.chars.resize(id + 1, nullptr);
		Character* d = renderGlyph(chars, id, size);
		chars.chars[id] = d;
		return d;
	}
	return nullptr;
}
size_t CFont::length(std::string text, long size) {
	size_t advance = 0;
	for (std::string::iterator i = text.begin(); i != text.end(); i++) {
		if (*i == ' ')
			advance += size / 1.6f;
		else {
			Character* ch = getCharacter(*i & 0x000000FF, size);
			if (ch)
				advance += ch->advanceX;
			else
				advance += size / 1.6f;
		}
	}
	return advance;
}
std::string CFont::trimStringToWidth(std::string& text, int width, long scale) {
	std::string s;
	size_t advance = 0;
	StringArray a = Parsing::splitString(text, " ");
	size_t length = 0;
	size_t space_width = CFont::length(" ", scale);
	for (int i = 0; i < a.size(); i++) {
		if (a[i].empty()) {
			s += " ";
			continue;
		}
		int l = CFont::length(a[i], scale);
		if (s.size())
			l += scale;

		if (length + l > width && !(l > width))
			return s;

		if (!s.empty()) {
			s += std::string(" ");
			length += space_width;
		}
		for (size_t j = 0; j < a[i].size(); j++) {
			if (length > width)
			{
				return s;
			}
			char c = a[i][j];
			if (c == '\n') {
				return s;
			}
			s += c;
			length += CFont::length(std::string(&c, 1), scale);
		}
	}
	return s;
}

std::vector<std::string> CFont::trimStringToMultiline(std::string text, int width, long scale) {
	std::vector<std::string> v;
	while (text.size()) {
		std::string l = trimStringToWidth(text, width, scale);
		v.push_back(l);
		if (l.size() + 1 >= text.size())
			break;
		text = text.substr(l.size() + 1);
	}
	return v;
}
#endif
