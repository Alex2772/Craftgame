#pragma once
#include <GL/glew.h>
#include <GL\GL.h>

class Framebuffer {
protected:
	virtual GLuint createFrameBuffer();
	virtual GLuint createTextureAttachment(size_t width, size_t height);
	virtual GLuint createDepthTextureAttachment(size_t width, size_t height);
	virtual GLuint createDepthBufferAttachment(size_t width, size_t height);
public:
	GLuint framebuffer;
	GLuint texture;
	GLuint depth;
	Framebuffer() {}
	Framebuffer(size_t width, size_t height);
	virtual ~Framebuffer();
	void bind(size_t width, size_t height);
	void unbind();
};