#pragma once

#include "gl.h"
#include "ImageLoader.h"

class GLTexture
{
private:
	GLuint texture;
public:
	GLTexture()
	{
		glGenTextures(1, &texture);
		bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
	void texImage2D(Image* image)
	{
		bind();
		GLint internalformat = 0;
		GLint format = 0;
		switch (image->bpp)
		{
		case 1:
			format = GL_RED;
			switch (image->type)
			{
			case GL_FLOAT:
				internalformat = GL_R16F;
				break;
			case GL_UNSIGNED_BYTE:
				internalformat = GL_R8;
				break;
			}
			break;
		case 3:
			format = GL_RGB;
			switch (image->type)
			{
			case GL_FLOAT:
				internalformat = GL_RGB16F;
				break;
			case GL_UNSIGNED_BYTE:
				internalformat = GL_RGB;
				break;
			}
			break;
		case 4:
			format = GL_RGBA;
			switch (image->type)
			{
			case GL_FLOAT:
				internalformat = GL_RGBA16F;
				break;
			case GL_UNSIGNED_BYTE:
				internalformat = GL_RGBA;
				break;
			}
			break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, image->width, image->height, 0, format, image->type, image->data);
	}
	~GLTexture()
	{
		glDeleteTextures(1, &texture);
	}
	void bind()
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	GLTexture(const GLTexture& t) = delete;
};
