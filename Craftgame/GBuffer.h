#pragma once

#include "gl.h"

class GBuffer
{
private:
	virtual GLuint createFrameBuffer();
	virtual GLuint createTextureAttachment(size_t width, size_t height, GLenum item);
	virtual GLuint createDepthTextureAttachment(size_t width, size_t height);
	virtual GLuint createDepthBufferAttachment(size_t width, size_t height);

public:
	enum Buffers
	{
		COLOR = 0,
		NORMAL = 1,
		F0 = 2,
		COUNT = 3,
	};
	GLuint framebuffer;
	GLuint buffers[Buffers::COUNT];
	GLuint depth;
	GBuffer(GLuint width, GLuint height);
	~GBuffer();
	void begin();
	void end();
	void bind(GLuint width, GLuint height);
	void bind();
	void unbind();
	void flush(bool lighting);
};