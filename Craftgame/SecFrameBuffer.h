#pragma once

#include <GL/glew.h>
#include <GL\GL.h>

class SecFramebuffer {
protected:
	GLuint createFrameBuffer();
	GLuint createTextureAttachment(GLuint width, GLuint height);
public:
	GLuint framebuffer;
	GLuint texture;
	SecFramebuffer(GLuint width, GLuint height);
	~SecFramebuffer();
	void begin();
	void end();
	void bind(GLuint width, GLuint height);
	void unbind();
};