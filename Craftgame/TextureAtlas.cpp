#ifndef SERVER
#include "TextureAtlas.h"
#include "GameEngine.h"

TextureAtlas::TextureAtlas(GLuint t, size_t _x, size_t _y):
	texture(t), x(_x), y(_y)
{
}

void TextureAtlas::generateUV(size_t id, glm::vec2& uv1, glm::vec2& uv2) const
{
	size_t xIndex = id % x;
	size_t yIndex = id / x;

	
	uv1.x = float(xIndex) / float(x);
	uv2.x = float(xIndex + 1) / float(x);

	uv1.y = float(yIndex) / float(y);
	uv2.y = float(yIndex + 1) / float(y);
}

/**
 * \brief Задаёт координаты UV по индексу иконки, в соответствии с размерами. Использовать в связке с GameRenderer::drawTextureAtlas.
 * \param id Номер иконки
 */
void TextureAtlas::bindTexture(size_t id) const
{
	CGE::instance->renderer->setTexturingMode(1);

	size_t uv = CGE::instance->guiShader->getUniform("uv");
	size_t uv2 = CGE::instance->guiShader->getUniform("uv2");

	glm::vec2 uuv1, uuv2;
	TextureAtlas::generateUV(id, uuv1, uuv2);

	CGE::instance->guiShader->loadVector(uv, uuv1);
	CGE::instance->guiShader->loadVector(uv2, uuv2);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void TextureAtlas::Generator::copy(Image* source, Image* target, size_t x, size_t y)
{
	// https://stackoverflow.com/questions/9900854/opengl-creating-texture-atlas-at-run-time
	for (int sourceY = 0; sourceY < source->height; ++sourceY) {
		for (int sourceX = 0; sourceX < source->width; ++sourceX) {
			int from = (sourceY * source->width * source->bpp) + (sourceX * source->bpp); // 4 bytes per pixel (assuming RGBA)
			int to = ((y + sourceY) * target->width * target->bpp) + ((x + sourceX) * target->bpp); // same format as source

			for (int channel = 0; channel < target->bpp; ++channel) {
				target->data[to + channel] = source->data[from + channel];
			}
		}
	}
}

TextureAtlas::Generator::Generator()
{
}

Image* TextureAtlas::Generator::generate(size_t& side)
{
	size_t width = mTextures[0]->width;
	side = ceilf(sqrtf(mTextures.size()));

	Image* atlas = new Image();
	atlas->width = atlas->height = side * width;
	atlas->alpha = false;
	atlas->data = new byte[atlas->width * atlas->height * 3];
	std::deque<Image*>::iterator it = mTextures.begin();
	for (size_t i = 0; i < mTextures.size(); ++i)
	{
		copy(*it, atlas, (i * width) % atlas->width, (i * width) / atlas->width * width);
		++it;
	}
	return atlas;
}

void TextureAtlas::Generator::addFragment(Image* f)
{
	mTextures.push_back(f);
}


#endif
