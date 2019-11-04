#pragma once
#include "gl.h"

class Depthbuffer {
private:
	size_t size;
	int layers;
protected:
	virtual GLuint createFrameBuffer();
	virtual GLuint createDepthTextureAttachment(size_t width, size_t height);
public:
	GLuint framebuffer;
	GLuint depth;
	Depthbuffer() {}
	inline GLenum getTextureType() {
		return layers > 1 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
	}
	Depthbuffer(size_t size, int layers = 1);
	virtual ~Depthbuffer();
	void bind();
	void unbind();
};