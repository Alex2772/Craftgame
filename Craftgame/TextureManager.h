#pragma once

#include <string>

#include "gl.h"
#include "ImageLoader.h"
#include <map>
#include "Res.h"
#include "ThreadUtils.h"

struct Texture {
	Image* image;
	GLuint texture;
};

typedef std::map<Resource, Texture> Textures;
class TextureManager {
private:
	ts<Textures> texture;
public:
	TextureManager(const TextureManager& m) {}
	TextureManager();
	~TextureManager();
	GLuint loadTexture(_R path, bool useTextureSmooth = false);
	void unloadTexture(_R path);
	void get(_R path, Texture& t);
	Image* load(_R path, bool useTextureSmooth);
	void load2(_R path, bool useTextureSmooth)
	{
		load(path, useTextureSmooth);
	}
	GLuint loadTexture(Image* data, Resource path, bool useTextureSmooth);
	GLuint loadTexture(Image* data, Resource path);
	void bind(_R r);
};