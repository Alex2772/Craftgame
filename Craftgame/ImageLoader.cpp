#ifndef SERVER
#pragma warning(disable: 4996)
#include "ImageLoader.h"
#include <string>
#include <exception>
#include "GameEngine.h"
#include "BitBuffer.h"

#include <stdio.h>
extern "C" {
#include <png.h>
#include <jpeglib.h>
}
Image::Image(uint8_t bpp):
	bpp(bpp)
{

}
Image::~Image() {
	delete[] data;
}

size_t Image::getDataSize()
{
	return size_t(width) * size_t(height) * bpp;
}

ImageLoader::ImageLoader() {

}
static std::map<png_structp, std::istream*> input;

void png_read_from_stream(png_structp png, png_bytep data, png_size_t length) {
	input[png]->read((char*)data, length);
}
Image* ImageLoader::loadPng(std::istream* _input) {
	unsigned int sig_read = 0;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	input[png_ptr] = _input;
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_info_struct(png_ptr, &info_ptr);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
	}
	png_set_read_fn(png_ptr, 0, png_read_from_stream);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	Image* i = new Image;
	int color_type;
	png_uint_32 jwidth;
	png_uint_32 jheight;
	png_get_IHDR(png_ptr, info_ptr, &jwidth, &jheight, nullptr, &color_type, nullptr, nullptr, nullptr);
	if (color_type == PNG_COLOR_TYPE_RGBA) {
		i->alpha = true;
		i->bpp = 4;
	}
	i->width = jwidth;
	i->height = jheight;
	int rowBytes = png_get_rowbytes(png_ptr, info_ptr);
	i->data = (byte*)malloc(rowBytes * i->height);
	png_bytepp rows = png_get_rows(png_ptr, info_ptr);
	for (int j = 0; j < i->height; j++)
		memcpy(i->data + rowBytes*(j), rows[j], rowBytes);

	input.erase(input.find(png_ptr));
	png_destroy_info_struct(png_ptr, &info_ptr);
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	return i;
}
struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
* Here's the routine that will replace the standard error_exit method:
*/

METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr)cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

struct JpegStream {
	jpeg_source_mgr pub;
	std::istream* stream;
	char buffer;
};
void init_source(j_decompress_ptr cinfo)
{
	auto src = (JpegStream*)(cinfo->src);
	src->stream->seekg(0); // seek to 0 here
}
boolean fill_buffer(j_decompress_ptr cinfo)
{
	// Read to buffer
	JpegStream* src = reinterpret_cast<JpegStream*>(cinfo->src);// as above
	
	src->stream->read(&src->buffer, sizeof(src->buffer));
	src->pub.next_input_byte = reinterpret_cast<byte*>(&src->buffer);
	src->pub.bytes_in_buffer = src->stream->gcount();// How many yo could read
	return src->stream->good();
}
void skip(j_decompress_ptr cinfo, long count)
{
	// Seek by count bytes forward
	// Make sure you know how much you have cached and subtract that
	// set bytes_in_buffer and next_input_byte
	reinterpret_cast<JpegStream*>(cinfo->src)->stream->seekg(count, std::ios_base::cur);
}
void term(j_decompress_ptr cinfo)
{
}
/*
* Шутка про ебучих шакалов
*/
Image* ImageLoader::loadJpeg(std::istream* path)
{
	struct my_error_mgr jerr;
	struct jpeg_decompress_struct cinfo; 
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		jpeg_destroy_decompress(&cinfo);
		WARN("Could not read jpeg file");
		return 0;
	}
	jpeg_create_decompress(&cinfo);
	JpegStream * src;
	if (cinfo.src == nullptr)
	{
		/* first time for this JPEG object? */
		cinfo.src = (struct jpeg_source_mgr *)
			(*cinfo.mem->alloc_small) (reinterpret_cast<j_common_ptr>(&cinfo), 0, sizeof(JpegStream));
		src = reinterpret_cast<JpegStream*> (cinfo.src);
	}
	src = reinterpret_cast<JpegStream*> (cinfo.src);
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_buffer;
	src->pub.skip_input_data = skip;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term;
	src->stream = path;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */

	(void)jpeg_read_header(&cinfo, TRUE);
	(void)jpeg_start_decompress(&cinfo);
	int row_stride = cinfo.output_width * cinfo.output_components;
	Image* img = new Image;
	img->width = cinfo.output_width;
	img->height = cinfo.output_height;
	img->data = new  unsigned char[img->width * img->height * 3];
	img->alpha = false;
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)(reinterpret_cast<j_common_ptr>(&cinfo), JPOOL_IMAGE, row_stride, 1);
	size_t row_stride_offset = 0;
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		* Here the array is only one element long, but you could ask for
		* more than one scanline at a time if that's more convenient.
		*/
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */
		memcpy(img->data + row_stride_offset, buffer[0], row_stride);
		row_stride_offset += row_stride;
	}
	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return img;
}

Image* ImageLoader::load(_R path)
{
	if (Parsing::endsWith(path.path, ".png"))
	{
		std::shared_ptr<std::istream> f = path.open();
		if (!f->good())
			return nullptr;
		return loadPng(f.get());
	}
	if (Parsing::endsWith(path.path, ".jpg") || Parsing::endsWith(path.path, ".jpeg"))
	{
		std::shared_ptr<std::istream> f = path.open();
		if (!f->good())
			return nullptr;
		return loadJpeg(f.get());
	} 
	WARN(std::string("Unsupported image format: ") + path.full);
	return nullptr;
}

void ImageLoader::savePng(Image* image, std::string path) {
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
/*
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
