#ifndef SERVER
#include "VAO.h"

GLuint VAO::genBuffer()
{
	GLuint b;
	glGenBuffers(1, &b);
	mBuffers.push_back(b);
	return b;
}

VAO::VAO()
{
	glGenVertexArrays(1, &vao);
}

VAO::~VAO()
{
	if (!mBuffers.empty())
		glDeleteBuffers(mBuffers.size(), mBuffers.data());
	glDeleteVertexArrays(1, &vao);
}

void VAO::draw(GLenum mode)
{
	assert(indices);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glDrawElements(mode, cIndices, GL_UNSIGNED_INT, nullptr);
}

void VAO::drawInstanced(GLenum mode, GLsizei count)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glDrawElementsInstanced(mode, cIndices, GL_UNSIGNED_INT, nullptr, count);
}

VAO::Loader::Loader(VAO& v)
	: vao(v)
{
	glBindVertexArray(vao.vao);
}

VAO::Loader::~Loader()
{
}

void VAO::Loader::store(GLuint attr, const std::vector<glm::vec2>& d, GLuint div)
{
	GLuint buf = vao.genBuffer();
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, d.size() * sizeof(glm::vec2), d.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attr, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(attr);
	if (div)
		glVertexAttribDivisor(attr, div);
}

void VAO::Loader::store(GLuint attr, const std::vector<glm::vec3>& d, GLuint div)
{
	GLuint buf = vao.genBuffer();
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, d.size() * sizeof(glm::vec3), d.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(attr);
	if (div)
		glVertexAttribDivisor(attr, div);
}
void VAO::Loader::store(GLuint attr, const std::vector<glm::vec4>& d, GLuint div)
{
	GLuint buf = vao.genBuffer();
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, d.size() * sizeof(glm::vec4), d.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(attr);
	if (div)
		glVertexAttribDivisor(attr, div);
}

void VAO::Loader::store(const std::vector<GLuint>& d)
{
	vao.indices = vao.genBuffer();
	vao.cIndices = d.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, d.size() * sizeof(GLuint), d.data(), GL_STATIC_DRAW);
}
#endif