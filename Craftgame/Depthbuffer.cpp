#include "Depthbuffer.h"
#include "GameEngine.h"
#ifndef SERVER
GLuint Depthbuffer::createFrameBuffer()
{
	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	return fb;
}

GLuint Depthbuffer::createDepthTextureAttachment(size_t width, size_t height)
{
	GLuint depth;
	glGenTextures(1, &depth);
	glBindTexture(getTextureType(), depth);
	if (layers > 1)
	{
		glTexImage3D(getTextureType(), 0, GL_DEPTH_COMPONENT32,
			width, height, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	} else
	{
		glTexImage2D(getTextureType(), 0, GL_DEPTH_COMPONENT24,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	}
	glTexParameteri(getTextureType(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(getTextureType(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(getTextureType(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(getTextureType(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(getTextureType(), GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(getTextureType(), GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glTexParameteri(getTextureType(), GL_TEXTURE_COMPARE_MODE, GL_NONE);

	return depth;
}

Depthbuffer::Depthbuffer(size_t s, int layers):
	size(s),
	layers(layers)
{
	assert(layers);
	framebuffer = createFrameBuffer();
	depth = createDepthTextureAttachment(s, s);
	//glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, getTextureType(), depth, 0, 2);
	//glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0, 0);
	//glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0, 1);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, getTextureType(), depth, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	unbind();
}

Depthbuffer::~Depthbuffer()
{
	glDeleteFramebuffers(1, &framebuffer);
	//glDeleteRenderbuffers(1, &depth);
	glDeleteTextures(1, &depth);
}

void Depthbuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, size, size);
}

void Depthbuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, CGE::instance->width, CGE::instance->height);
}
#endif