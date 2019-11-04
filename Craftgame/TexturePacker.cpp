#ifndef SERVER
#include "TexturePacker.h"
#include "CraftgameException.h"
#include "ImageLoader.h"
#include <boost/smart_ptr/make_shared.hpp>
#include <Windows.h>

TexturePacker::Rect::Rect(dim x, dim y, dim width, dim height)
	: x(x),
	y(y),
	width(width),
	height(height)
{
}

bool TexturePacker::Rect::hasPoint(dim tx, dim ty) const
{
	return tx >= x && tx < (x + width) && ty >= y && ty < (y + height);
}


bool TexturePacker::noCollision(const Rect& r)
{
	if (r.x < 0 || r.y < 0 || r.x + r.width > side || r.y + r.height > side)
	{
		return false;
	}
	for (Rect& t : mRects)
	{
		if (t.hasPoint(r.x, r.y) ||
			t.hasPoint(r.x + r.width - 1, r.y) ||
			t.hasPoint(r.x + r.width - 1, r.y + r.height - 1) ||
			t.hasPoint(r.x, r.y + r.height - 1) || 
			r.hasPoint(t.x, t.y) ||
			r.hasPoint(t.x + t.width - 1, t.y) ||
			r.hasPoint(t.x + t.width - 1, t.y + t.height - 1) ||
			r.hasPoint(t.x, t.y + t.height - 1))
		{
			return false;
		}
	}
	return true;
}

bool TexturePacker::check(Rect& res, Rect r)
{
	if (noCollision(r))
	{
		res = r;
		mRects.push_back(res);
		return true;
	}
	return false;
}

TexturePacker::TexturePacker(uint8_t bpp):
side(64)
{
	mImage = new Image(bpp);
	mImage->width = mImage->height = side;
	mImage->data = new uint8_t[mImage->getDataSize()];
	memset(mImage->data, 0, mImage->getDataSize());
}

TexturePacker::~TexturePacker()
{
	delete mImage;
}

void TexturePacker::resize(size_t size)
{
	float k = size / side;
	for (auto& i : uvs)
	{
		i->x /= k;
		i->y /= k;
		i->z /= k;
		i->a /= k;
	}
	side = size;
	Image* kx = new Image;
	kx->bpp = mImage->bpp;
	kx->width = kx->height = side;
	size_t s = kx->getDataSize();
	kx->data = new uint8_t[s];
	memset(kx->data, size, s);
	TextureAtlas::Generator::copy(mImage, kx, 0, 0);
	delete mImage;
	mImage = kx;
}

boost::shared_ptr<glm::vec4> TexturePacker::insert(Image* img)
{
	Rect r(0, 0, 0, 0);
	while (!allocateRect(r, img->width, img->height))
	{
		resize(side * 2);
	}
	TextureAtlas::Generator::copy(img, mImage, r.x, r.y);
	boost::shared_ptr<glm::vec4> f = boost::make_shared<glm::vec4>(float(r.x) / float(side), float(r.y) / float(side) , float(r.x + r.width) / float(side) , float(r.y + r.height) / float(side));
	uvs.push_back(f);
	return f;
}

bool TexturePacker::allocateRect(Rect& t, dim width, dim height)
{
	// Выделение
	if (mRects.empty())
	{
		t = Rect(0, 0, width, height); // Тупо в (0, 0) влепить
		mRects.push_back(t);
		return true;
	}

	for (Rect& r : mRects)
	{
		if (check(t, Rect(r.x + r.width, r.y, width, height)))
		{
			return true;
		}
		if (check(t, Rect(r.x, r.y + r.height, width, height)))
		{
			return true;
		}
		if (check(t, Rect(r.x + r.width, r.y + r.height - height, width, height)))
		{
			return true;
		}

		if (check(t, Rect(r.x + r.width - width, r.y + r.height, width, height)))
		{
			return true;
		}

		if (check(t, Rect(r.x - width, r.y, width, height)))
		{
			return true;
		}
		if (check(t, Rect(r.x - width, r.y + r.height - height, width, height)))
		{
			return true;
		}

		if (check(t, Rect(r.x + r.width - width, r.y - height, width, height)))
		{
			return true;
		}
		if (check(t, Rect(r.x, r.y - height, width, height)))
		{
			return true;
		}
	}

	return false;
}
#endif