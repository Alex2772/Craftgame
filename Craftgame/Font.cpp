#ifndef SERVER
#pragma warning(disable: 4996)
#include "Font.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include "GameEngine.h"


Font::Font(std::string _path, _R& _res) :
	res(_res),
	path(_path)
{
	if (FT_New_Face(CGE::instance->ft, _path.c_str(), 0, &face)) {
		throw std::runtime_error(std::string("Failed to load font ") + _path + ": FT_Face could not be created");
	}
}
Font::~Font() {
	FT_Done_Face(face);
	for (size_t i = 0; i < data.size(); i++) {
		std::vector<Character*> c = data[i].chars;
		for (size_t j = 0; j < c.size(); j++) {
			Character* ch = c[j];
			delete[] ch->data;
			glDeleteTextures(1, &ch->texture);
			delete ch;
		}
		c.clear();
	}
	data.clear();
}
std::vector<Character*>& Font::getCharsetBySize(long size) {
	for (size_t i = 0; i < data.size(); i++) {
		if (data[i].size == size)
			return data[i].chars;
	}
	Font_Size s;
	s.size = size;
	data.push_back(s);
	//renderGlyphs(size);
	return data[data.size() - 1].chars;
}
Character* Font::renderGlyph(FT_ULong glyph, long size) {
	char s = glyph;
	std::wstring wc(sizeof(s), L'#');
	mbstowcs(&wc[0], (char*)&s, sizeof(s));
	FT_Set_Pixel_Sizes(face, 0, size);
	if (FT_Load_Char(face, wc[0], FT_LOAD_RENDER)) {
		throw std::runtime_error(std::string("Cannot load char ") + std::to_string(glyph));
	}
	FT_GlyphSlot g = face->glyph;
	if (g->bitmap.width && g->bitmap.rows) {
		Character* c = new Character;
		c->c = glyph;
		c->width = g->bitmap.width;
		c->height = g->bitmap.rows;
		c->advanceX = g->advance.x >> 6;
		c->advanceY = -(g->metrics.horiBearingY >> 6) + size;
		c->data = new unsigned char[g->bitmap.width * g->bitmap.rows];
		memcpy(c->data, g->bitmap.buffer, c->width * c->height);
		glGenTextures(1, &c->texture);
		glBindTexture(GL_TEXTURE_2D, c->texture); 
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, c->width, c->height, 0, GL_RED, GL_UNSIGNED_BYTE, c->data);
		return c;
	}
	return nullptr;
}

Character* Font::getCharacter(FT_ULong id, long size) {
	std::vector<Character*>& chars = getCharsetBySize(size);
	for (std::vector<Character*>::iterator i = chars.begin(); i != chars.end(); i++) {
		if ((*i)->c == id) {
			return *i;
		}
	}
	Character* d = renderGlyph(id, size);
	if (d) {
		chars.push_back(d);
		return d;
	}
	return nullptr;
}
size_t Font::length(std::string text, long size) {
	size_t advance = 0;
	for (std::string::iterator i = text.begin(); i != text.end(); i++) {
		if (*i == ' ')
			advance += size / 1.6f;
		else {
			Character* ch = getCharacter(*i, size);
			if (ch)
				advance += ch->advanceX;
			else
				advance += size / 1.6f;
		}
	}
	return advance;
}
StringArray Font::split(const std::string &s, char delim) {
	StringArray vect;
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim)) {
		if (item.length() > 0)
			vect.push_back(item);
	}
	return vect;
}
std::string Font::trimStringToWidth(std::string& text, int width, long scale) {
	std::string s;
	size_t advance = 0;
	StringArray a = split(text, ' ');
	long length = 0;
	for (int i = 0; i < a.size(); i++) {
		int l = Font::length(a[i], scale);
		if (s.size())
			l += scale + 1;
		if (length + l > width) {
			break;
		}

		if (s.size()) 
			s += std::string(" ") + a[i];
		else
			s += a[i];
		length += l;
	}
	return s;
}

std::vector<std::string> Font::trimStringToMultiline(std::string text, int width, long scale) {
	std::vector<std::string> v;
	while (text.size()) {
		std::string l = trimStringToWidth(text, width);
		v.push_back(l);
		if (l.size() + 1 >= text.size())
			break;
		text = text.substr(l.size() + 1);
	}
	return v;
}
#endif