#ifndef SERVER
#include "TextureManager.h"
#include "GameEngine.h"

TextureManager::TextureManager() {

}
TextureManager::~TextureManager() {
	for (Textures::iterator i = texture.begin(); i != texture.end(); i++) {
		glDeleteTextures(1, &(i->texture));
		delete i->image;
	}
}
GLuint TextureManager::loadTexture(std::string path, bool useTextureSmooth) {
	for (Textures::iterator i = texture.begin(); i != texture.end(); i++) {
		if (i->path == path)
			return i->texture;
	}
	Image* image = CGE::instance->imageLoader->loadPNG(path);
	return loadTexture(image, path);
}
GLuint TextureManager::loadTexture(Image* image, std::string path, bool useTextureSmooth) {
	GLuint t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	if (image->alpha)
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image->width, image->height, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
	else
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->data);
	if (useTextureSmooth) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	Texture tex;
	tex.image = image;
	tex.path = path;
	tex.texture = t;
	texture.push_back(tex);
	return t;
}
#endif