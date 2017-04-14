#ifndef SERVER
#include "Framebuffer.h"
#include <GL/glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include "GameEngine.h"

Framebuffer::Framebuffer(size_t width, size_t height) {
	framebuffer = createFrameBuffer();
	texture = createTextureAttachment(width, height);
	depth = createDepthBufferAttachment(width, height);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	unbind();
}
Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texture);
	glDeleteRenderbuffers(1, &depth);
}
GLuint Framebuffer::createFrameBuffer() {
	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	return fb;
}
GLuint Framebuffer::createTextureAttachment(size_t width, size_t height) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
	return texture;
}
GLuint Framebuffer::createDepthBufferAttachment(size_t width, size_t height) {
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, depthBuffer);

	return depthBuffer;
}
GLuint Framebuffer::createDepthTextureAttachment(size_t width, size_t height) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	glDrawBuffer(GL_NONE); // No color buffer is drawn to.
	return texture;
}

void Framebuffer::bind(size_t width, size_t height) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, width, height);
}
void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, CGE::instance->width, CGE::instance->height);
}
#endif