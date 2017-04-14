#pragma once

#include <string>

class Image {
public:
	Image();
	~Image();
	unsigned int width;
	unsigned int height;
	unsigned char* data;
	bool alpha = false;
};

class ImageLoader {
public:
	ImageLoader();
	Image* loadPNG(std::string path);
	Image* loadPNG(std::istream& input);
	//Image* loadGIF(std::string path);
	void savePNG(Image* image, std::string path);
};