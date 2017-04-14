#pragma once

#include <string>
#include <GL/glew.h>
#include <GL\GL.h>
#include "ImageLoader.h"
#include <vector>

struct Texture {
	Image* image;
	GLuint texture;
	std::string path;
};

typedef std::vector<Texture> Textures;
class TextureManager {
private:
	Textures texture;
public:
	TextureManager();
	~TextureManager();
	GLuint loadTexture(std::string path, bool useTextureSmooth = false);
	GLuint loadTexture(Image* data, std::string path, bool useTextureSmooth = false);
};