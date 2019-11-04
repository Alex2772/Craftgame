#pragma once

#include "Res.h"
#include  "gl.h"
class Image {
public:
	uint8_t bpp; // Bytes Per Pixel
	Image(uint8_t bpp = 3);
	~Image();
	size_t getDataSize();
	unsigned int width;
	unsigned int height;
	unsigned char* data;
	bool alpha = false;
	GLenum type = GL_UNSIGNED_BYTE;
};

class ImageLoader {
public:
	ImageLoader();
	Image* loadPng(std::istream* input);
	Image* loadJpeg(std::istream* path);
	Image* load(_R path);
	//Image* loadGIF(std::string path);
	void savePng(Image* image, std::string path);
};
