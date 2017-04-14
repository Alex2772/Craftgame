#ifndef SERVER
#pragma warning(disable: 4996)
#include "ImageLoader.h"
#include <string>
#include <exception>
#include "png.h"
#include "GameEngine.h"
#include "BitBuffer.h"

Image::Image() {

}
Image::~Image() {
	delete[] data;
}

ImageLoader::ImageLoader() {

}
Image* ImageLoader::loadPNG(std::string path) {
	FILE* fp = fopen(path.c_str(), "rb");
	if (!fp)
		throw  CraftgameException(("Cannot open file " + path).c_str());

	unsigned int sig_read = 0;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_info_struct(png_ptr, &info_ptr);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		throw  CraftgameException("png_jmpbuf");
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	Image* i = new Image;
	int color_type;
	png_get_IHDR(png_ptr, info_ptr, &i->width, &i->height, nullptr, &color_type, nullptr, nullptr, nullptr);
	if (color_type == PNG_COLOR_TYPE_RGBA)
		i->alpha = true;
	int rowBytes = png_get_rowbytes(png_ptr, info_ptr);
	i->data = (byte*)malloc(rowBytes * i->height);
	png_bytepp rows = png_get_rows(png_ptr, info_ptr);
	for (int j = 0; j < i->height; j++)
		memcpy(i->data + rowBytes*(j), rows[j], rowBytes);

	png_destroy_info_struct(png_ptr, &info_ptr);
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	fclose(fp);
	return i;
}
static std::istream* input;

void png_read_from_stream(png_structp png, png_bytep data, png_size_t length) {
	input->read((char*)data, length);
}
Image* ImageLoader::loadPNG(std::istream& _input) {
	input = &_input;
	unsigned int sig_read = 0;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_info_struct(png_ptr, &info_ptr);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		throw  CraftgameException("png_jmpbuf");
	}
	png_set_read_fn(png_ptr, 0, png_read_from_stream);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	Image* i = new Image;
	int color_type;
	png_get_IHDR(png_ptr, info_ptr, &i->width, &i->height, nullptr, &color_type, nullptr, nullptr, nullptr);
	if (color_type == PNG_COLOR_TYPE_RGBA)
		i->alpha = true;
	int rowBytes = png_get_rowbytes(png_ptr, info_ptr);
	i->data = (byte*)malloc(rowBytes * i->height);
	png_bytepp rows = png_get_rows(png_ptr, info_ptr);
	for (int j = 0; j < i->height; j++)
		memcpy(i->data + rowBytes*(j), rows[j], rowBytes);

	png_destroy_info_struct(png_ptr, &info_ptr);
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	return i;
}

void ImageLoader::savePNG(Image* image, std::string path) {
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	FILE* fp = nullptr;
	fp = fopen(path.c_str(), "wb");
	if (fp == nullptr) {
		throw  CraftgameException(("Cannot save file " + path).c_str());
	}
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	info_ptr = png_create_info_struct(png_ptr);

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_info_struct(png_ptr, &info_ptr);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		throw  CraftgameException("Error during PNG creation");
	}
	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, info_ptr, image->width, image->height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);

	for (int y = 0; y<image->height; y++) {
		png_write_row(png_ptr, (png_bytep)(&(image->data[y * image->width * 3])));
	}

	// End write
	png_write_end(png_ptr, NULL);
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
}
#include "BinaryInputStream.h"
#include <bitset>
#include <vector>

typedef unsigned short ushort;

struct gif_screen {
	ushort width;
	ushort height;
	byte params;
	byte backgroundIndex;
	byte k;
};
struct gif_color {
	byte r;
	byte g;
	byte b;
};
struct gif_image_descriptor {
	ushort x;
	ushort y;
	ushort width;
	ushort height;
	byte data;
};
size_t pow(size_t b, size_t p) {
	for (size_t i = 1; i < p; i++) {
		b *= b;
	}
	return b;
}
/*
Image* ImageLoader::loadGIF(std::string path) {
	BinaryInputStream p(path);
	if (!p.good())
		throw  CraftgameException(("Cannot open file " + path).c_str());

	// Проверка заголовка
	char header[7];
	header[6] = '\0';
	p.ifstream::read(header, 6);
	if (strcmp(header, "GIF87a") && strcmp(header, "GIF89a"))
		throw  CraftgameException(("File " + path + " is not GIF.").c_str());
	
	// Загрузка информации об экране
	gif_screen screen;
	p.ifstream::read((char*)&screen, 7);
	std::bitset<8> params(screen.params);
	if (!params[7])
		throw  CraftgameException((path + " doesn't use global color table").c_str());
	if (screen.k) {
		throw  CraftgameException((path + " k is not 0").c_str());
	}

	// Формирование таблицы цветов
	std::vector<gif_color> color;
	size_t cts = pow(2, byte(screen.params & 7) + 1);
	color.reserve(cts);
	for (size_t i = 0; i < cts; i++) {
		gif_color c;
		c = p.read<gif_color>();
		color.push_back(c);
	}


	if (p.read<byte>() != 0x2c) {
		throw  CraftgameException("Ты охуел? где описание изображения? иди нахуй со своими гифками блять");
	}
	std::vector<std::vector<size_t>> dict;
	for (size_t i = 0; i < color.size(); i++) {
		dict.push_back({ i });
	}
	dict.push_back({ 0 });
	dict.push_back({ 0 });

	// Секция описания изображения
	gif_image_descriptor desc;
	p.ifstream::read((char*)&desc, 9);;
	
	ushort clear = color.size();
	ushort end = clear + 1;

	byte lzw_length = p.read<byte>() + 1;
	std::vector<size_t> indices;
	bool crt = true;
	while (p.good() && crt) {
		byte lzw_data_length = p.read<byte>();
		BitBuffer bitBuffer;
		char* buffer = new char[lzw_data_length];
		p.ifstream::read(buffer, lzw_data_length);
		bitBuffer.write((unsigned char*)buffer, lzw_data_length);
		delete[] buffer;
		int prevIndex = -1;
		while (bitBuffer.seekg() < lzw_data_length * 8 - 1) {
			if (dict.size() >= pow(2, lzw_length) - 1) {
				lzw_length++;
			}
			size_t b = bitBuffer.read(lzw_length);
			if (b == clear) {
				continue;
			}
			else if (b == end) {
				crt = false;
				break;
			} else {
				std::vector<size_t> cb = dict[b];
				for (size_t i = 0; i < cb.size(); i++) {
					indices.push_back(cb[i]);
				}
				if (prevIndex >= 0) {
					std::vector<size_t> p = dict[prevIndex];
					std::vector<size_t> v;
					for (size_t i = 0; i < p.size(); i++) {
						v.push_back(p[i]);
					}
					std::vector<size_t> p2 = dict[dict.size() - 1];
					v.push_back(p2[0]);
					dict.push_back(v);
				}
				prevIndex = b;
			}
		}
		
	}
	p.close();
	return nullptr;
}*/
#endif