#pragma once


#include "gl.h"
#include <glm/glm.hpp>
#include "ImageLoader.h"
#include <deque>

class TextureAtlas
{
private:
	size_t x;
	size_t y;
public:
	GLuint texture;
	TextureAtlas() {}
	TextureAtlas(GLuint texture, size_t x, size_t y);
	void generateUV(size_t id, glm::vec2& uv1, glm::vec2& uv2) const;
	void bindTexture(size_t id) const;

	class Generator
	{
	private:
		std::deque<Image*> mTextures;
	public:
		Generator();
		static void copy(Image* source, Image* target, size_t x, size_t y);
		Image* generate(size_t& s);
		void addFragment(Image*);
	};
};
