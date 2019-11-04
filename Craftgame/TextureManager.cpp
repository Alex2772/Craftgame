#ifndef SERVER
#include "TextureManager.h"
#include "GameEngine.h"

TextureManager::TextureManager():
	texture(new Textures)
{
}
TextureManager::~TextureManager() {
	ts<Textures>::rw texture = TextureManager::texture;
	for (Textures::iterator i = texture->begin(); i != texture->end(); i++) {
		glDeleteTextures(1, &(i->second.texture));
		delete i->second.image;
	}
}

void TextureManager::bind(_R r)
{
	glBindTexture(GL_TEXTURE_2D, loadTexture(r, false));
}

GLuint TextureManager::loadTexture(_R path, bool useTextureSmooth) {
	ts<Textures>::rw texture = TextureManager::texture;
	Textures::iterator it = texture->find(path);
	if (it != texture->end())
	{
		return it->second.texture;
	}
	try {
		Image* image = CGE::instance->imageLoader->load(path);
		if (!image)
			throw CraftgameException(this, std::string("Image is not loaded: ") + path.full);
		return loadTexture(image, path, useTextureSmooth);
	}
	catch (CraftgameException e) {
		WARN(e.what());
		return loadTexture(_R("craftgame:notexture"), false);
	}
}

Image* TextureManager::load(_R path, bool useTextureSmooth)
{
	try {
		Image* image = CGE::instance->imageLoader->load(path);
		if (!image)
			throw CraftgameException(this);
		CGE::instance->uiThread.push([&, image, path]()
		{
			loadTexture(image, path, useTextureSmooth);
		});
		return image;
	}
	catch (CraftgameException e) {
		WARN(e.what());
	}
	return nullptr;
}

void TextureManager::unloadTexture(_R path) {
	ts<Textures>::rw texture = TextureManager::texture;
	Textures::iterator f = texture->find(path);
	if (f != texture->end())
	{
		glDeleteTextures(1, &f->second.texture);
		delete f->second.image;
		texture->erase(f);
	}
}

void TextureManager::get(_R path, Texture& t) {
	ts<Textures>::r texture = TextureManager::texture;
	Textures::iterator f = texture->find(path);
	if (f != texture->end())
		t = f->second;
}
GLuint TextureManager::loadTexture(Image* image, _R path) {
	return loadTexture(image, path, image->width >= 256);
}
GLuint TextureManager::loadTexture(Image* image, _R path, bool useTextureSmooth) {
	bool mipmap = path.path.find("res/gui/") != 0;


	GLuint t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	if (mipmap) {
		if (image->alpha)
			gluBuild2DMipmaps(GL_TEXTURE_2D, image->type == GL_FLOAT ? GL_RGBA16F : GL_RGBA, image->width, image->height, GL_RGBA, image->type, image->data);
		else
			gluBuild2DMipmaps(GL_TEXTURE_2D, image->type == GL_FLOAT ? GL_RGB16F : GL_RGB, image->width, image->height, GL_RGB, image->type, image->data);
	} else
	{
		if (image->alpha)
			glTexImage2D(GL_TEXTURE_2D, 0, image->type == GL_FLOAT ? GL_RGBA16F : GL_RGBA, image->width, image->height, 0, GL_RGBA, image->type, image->data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, image->type == GL_FLOAT ? GL_RGB16F : GL_RGB, image->width, image->height, 0, GL_RGB, image->type, image->data);
	}
	if (useTextureSmooth) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	if (mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);
	Texture tex;
	tex.image = image;
	tex.texture = t;

	ts<Textures>::rw texture = TextureManager::texture;
	(*texture.get())[path] = tex;
	return t;
}
#endif
