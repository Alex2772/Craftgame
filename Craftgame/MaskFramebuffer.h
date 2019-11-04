#pragma once


#include "gl.h"

class MaskFramebuffer {
protected:
	virtual GLuint createFrameBuffer();
	virtual GLuint createTextureAttachment(size_t width, size_t height);
public:
	GLuint texture;
	GLuint framebuffer;
	MaskFramebuffer() {}
	MaskFramebuffer(size_t width, size_t height);
	virtual ~MaskFramebuffer();
	void bind(size_t width, size_t height);
	void unbind();
	void begin();
	void end();
};