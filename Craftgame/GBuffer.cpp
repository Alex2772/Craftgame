#include "Random.h"
#ifndef SERVER
#include "GBuffer.h"
#include "GameEngine.h"
#include "Models.h"

GBuffer::GBuffer(GLuint width, GLuint height) {
	framebuffer = createFrameBuffer();
	depth = createDepthTextureAttachment(width, height);
	for (GLenum i = 0; i < COUNT; i++)
		buffers[i] = createTextureAttachment(width, height, GL_COLOR_ATTACHMENT0 + i);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	unbind();
}
GBuffer::~GBuffer() {
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(COUNT, buffers);
	glDeleteTextures(1, &depth);
}

void GBuffer::begin()
{
	bind(CGE::instance->width, CGE::instance->height);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
}

void GBuffer::end()
{
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	for (byte i = 0; i < COUNT; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, buffers[i]);
	}
	glActiveTexture(GL_TEXTURE0);
	CGE::instance->gbufferShader[0]->start();
	CGE::instance->renderer->renderModel(Models::viewport);
	bind(CGE::instance->width, CGE::instance->height);
}

GLuint GBuffer::createFrameBuffer() {
	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	unsigned int attachments[9] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6,GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8 };
	glDrawBuffers(COUNT, attachments);
	return fb;
}
GLuint GBuffer::createTextureAttachment(size_t width, size_t height, GLenum item) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	bool kek = false;
	glTexImage2D(GL_TEXTURE_2D, 0, kek ? GL_RGBA16F : GL_RGBA, width, height, 0, GL_RGBA, kek ? GL_FLOAT : GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glFramebufferTexture(GL_FRAMEBUFFER, item, texture, 0);
	return texture;
}
GLuint GBuffer::createDepthBufferAttachment(size_t width, size_t height) {
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, depthBuffer);

	return depthBuffer;
}
GLuint GBuffer::createDepthTextureAttachment(size_t width, size_t height) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	//glDrawBuffer(GL_NONE); // No color buffer is drawn to.
	return texture;
}

void GBuffer::bind(GLuint width, GLuint height) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, width, height);
}
void GBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, CGE::instance->width, CGE::instance->height);
}

extern glm::mat4 lightSpaceMatrix1;
extern glm::mat4 lightSpaceMatrix2;
void GBuffer::flush(bool lighting)
{
	CGE::instance->secFB->bind(CGE::instance->width, CGE::instance->height);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	CGE::instance->currentShader = nullptr;
	CGE::instance->gbufferShader[0]->start();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CGE::instance->gBuffer->depth);
	for (GLenum i = 0; i < GBuffer::COUNT; i++) {
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D, CGE::instance->gBuffer->buffers[i]);
	}
	if (CGE::instance->shadowFB) {
		CGE::instance->gbufferShader[0]->loadInt("shadows", 1);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D_ARRAY, CGE::instance->shadowFB->depth);
	}
	else {
		CGE::instance->gbufferShader[0]->loadInt("shadows", 0);
	}

	static GLuint _noise = CGE::instance->textureManager->loadTexture("craftgame:ssao4x4");
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, _noise);
	glActiveTexture(GL_TEXTURE0);

	glm::mat4 view = CGE::instance->camera->createViewMatrix();


	CGE::instance->gbufferShader[0]->loadMatrix("tr", glm::mat4(1.0));
	CGE::instance->gbufferShader[0]->loadMatrix("InverseViewProjection", glm::mat4(1.0));
	CGE::instance->gbufferShader[0]->loadVector("light.pos", glm::vec3(0));
	CGE::instance->gbufferShader[0]->loadVector("light.color", glm::vec3(0));
	CGE::instance->gbufferShader[0]->loadInt("addtive", 0);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CGE::instance->renderer->renderModel(Models::viewport);
	CGE::instance->gbufferShader[0]->loadInt("addtive", 1);
	// Загрузка данных в шейдеры
	for (size_t i = 0; i < CGE::instance->gbufferShader.size(); i++) {
		CGE::instance->gbufferShader[i]->use();
		CGE::instance->gbufferShader[i]->loadMatrix("tr", glm::mat4(1.0));
		CGE::instance->gbufferShader[i]->loadVector("viewPos", glm::vec3(CGE::instance->camera->pos));
		CGE::instance->gbufferShader[i]->loadMatrix("ViewProjection", CGE::instance->projection * view);
		CGE::instance->gbufferShader[i]->loadMatrix("View", view);
		CGE::instance->gbufferShader[i]->loadMatrix("InverseViewProjection", glm::inverse(CGE::instance->projection * view));
		CGE::instance->gbufferShader[i]->loadMatrix("InverseProjection", glm::inverse(CGE::instance->projection));
	}
	CGE::instance->gbufferShader[0]->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	if (lighting) {
		if (CGE::instance->thePlayer) {
			// Солнце
			CGE::instance->gbufferShader[0]->loadInt("sun", 1);
			CGE::instance->gbufferShader[0]->loadMatrix("lightSpaceMatrix1", lightSpaceMatrix1);
			CGE::instance->gbufferShader[0]->loadMatrix("lightSpaceMatrix2", lightSpaceMatrix2);
			glm::vec3 pos = CGE::instance->thePlayer->worldObj->calculateSunPos();
			//float _y = pos.y;
			//pos.y = glm::abs(pos.y);
			CGE::instance->gbufferShader[0]->loadVector("light.pos", CGE::instance->thePlayer->getPos().toVec3() + pos * 1000.f);
			CGE::instance->gbufferShader[0]->loadVector("light.color", glm::vec3(1.f, 0.99f, 0.76f) * 5.f);
			CGE::instance->gbufferShader[0]->loadMatrix("tr", glm::mat4(1.0));
			CGE::instance->renderer->renderModel(Models::viewport);
			CGE::instance->gbufferShader[0]->loadInt("sun", 0);
		}

		glCullFace(GL_FRONT);
		static AdvancedCGEMModel* model = dynamic_cast<AdvancedCGEMModel*>(CGE::instance->modelRegistry->getModel("sphere"));
		std::vector<ModelObject*>::iterator it = model->objects.begin();
		glBindVertexArray((*it)->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*it)->i);
		glEnableVertexAttribArray(0);
		ts<std::vector<LightSource*>>::rw v(CGE::instance->light);
		size_t _light_cnt = 0;
		for (LightSource* i : *v.get())
		{
			if (!i)
				continue;
			i->tick();
			if (CGE::instance->renderer->frustum.isSphereInFrustum(i->pos, i->distance)) {
				CGE::instance->gbufferShader[0]->loadVector("light.pos", i->pos);
				CGE::instance->gbufferShader[0]->loadVector("light.color", i->color);
				CGE::instance->gbufferShader[0]->loadFloat("light.distance", i->distance);
				CGE::instance->gbufferShader[0]->loadMatrix("tr", CGE::instance->projection * view * glm::translate(glm::scale(glm::mat4(1.0), glm::vec3(i->distance)), i->pos / i->distance));
				glDrawElements(GL_TRIANGLES, (GLsizei)(*it)->indices->size(), GL_UNSIGNED_INT, (void*)0);
				_light_cnt++;
			}
		}
		CGE::instance->setDebugString("Light", std::to_string(_light_cnt));
		glDisableVertexAttribArray(0);
		glCullFace(GL_BACK);
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, CGE::instance->secFB->texture);
	glActiveTexture(GL_TEXTURE0);
	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
	// Шейдер постобработки
	CGE::instance->gbufferShader[1]->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < 3; i++)
		view[3][i] = 0.f;
	view[3][3] = 1.f;
	CGE::instance->gbufferShader[1]->loadMatrix("vp", glm::inverse(view) * glm::inverse(CGE::instance->projection));
	CGE::instance->gbufferShader[1]->loadVector("sun", CGE::instance->thePlayer ? CGE::instance->thePlayer->worldObj->calculateSunPos() : glm::vec3(1, 1, 1));
	CGE::instance->renderer->renderModel(Models::viewport);

	CGE::instance->mainFB->bind(CGE::instance->width, CGE::instance->height);
}
#endif
