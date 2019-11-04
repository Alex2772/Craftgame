#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "TextureAtlas.h"
#include <boost/smart_ptr/shared_ptr.hpp>

class Image;

class TexturePacker
{
private:
	size_t side;
	typedef int dim;
	class Rect
	{
	public:
		dim x, y, width, height;
		Rect(dim x, dim y, dim width, dim height);
		bool hasPoint(dim x, dim y) const;
	};

	std::vector<Rect> mRects;
	std::vector<boost::shared_ptr<glm::vec4>> uvs;
	bool noCollision(const Rect& r);
	bool check(Rect& res, Rect r);
	bool allocateRect(Rect& r, dim width, dim height);
public:
	Image* mImage = nullptr;
	TexturePacker(uint8_t bpp = 3);
	~TexturePacker();
	void resize(size_t size);
	boost::shared_ptr<glm::vec4> insert(Image* img);
};
