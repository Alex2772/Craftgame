#ifndef SERVER
#include "MaskFramebuffer.h"
#include <GL/glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include "GameEngine.h"
#include "Models.h"

MaskFramebuffer::MaskFramebuffer(size_t width, size_t height) {
	framebuffer = createFrameBuffer();
	texture = createTextureAttachment(width, height);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	unbind();
}
MaskFramebuffer::~MaskFramebuffer() {
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texture);
}
GLuint MaskFramebuffer::createFrameBuffer() {
	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	return fb;
}
GLuint MaskFramebuffer::createTextureAttachment(size_t width, size_t height) {
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

void MaskFramebuffer::bind(size_t width, size_t height) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, width, height);
}
void MaskFramebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, CGE::instance->width, CGE::instance->height);
}
void MaskFramebuffer::begin() {
	bind(CGE::instance->width, CGE::instance->height);
	glClear(GL_COLOR_BUFFER_BIT);
}
void MaskFramebuffer::end() {
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
}
#endif