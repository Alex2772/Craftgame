#ifndef SERVER
#include "SecFramebuffer.h"
#include "GameEngine.h"
#include "Models.h"

SecFramebuffer::SecFramebuffer(GLuint width, GLuint height) {
	framebuffer = createFrameBuffer();
	texture = createTextureAttachment(width, height);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	unbind();
}
SecFramebuffer::~SecFramebuffer() {
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texture);
}
void SecFramebuffer::begin() {
	bind(CGE::instance->width, CGE::instance->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void SecFramebuffer::end() {
	GLuint transform = CGE::instance->guiShader->getUniform("transform");
	GLuint c = CGE::instance->guiShader->getUniform("c");
	GLuint uv = CGE::instance->guiShader->getUniform("uv");
	GLuint uv2 = CGE::instance->guiShader->getUniform("uv2");
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	glBindTexture(GL_TEXTURE_2D, texture);
	CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 1));
	CGE::instance->guiShader->loadVector(uv, glm::vec2(0, 0));
	CGE::instance->guiShader->loadVector(uv2, glm::vec2(1, 1));
	CGE::instance->guiShader->loadMatrix(transform, glm::mat4(1));
	CGE::instance->renderer->renderModel(Models::viewport);
	bind(CGE::instance->width, CGE::instance->height);
	glClear(GL_COLOR_BUFFER_BIT);
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
}
GLuint SecFramebuffer::createFrameBuffer() {
	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	return fb;
}
GLuint SecFramebuffer::createTextureAttachment(GLuint width, GLuint height) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
	return texture;
}

void SecFramebuffer::bind(GLuint width, GLuint height) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, width, height);
}
void SecFramebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	bind(CGE::instance->width, CGE::instance->height);
}
void SecFramebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, CGE::instance->width, CGE::instance->height);
}
#endif