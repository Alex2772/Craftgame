#pragma once

#include "gl.h"
#include <glm/glm.hpp>
#include <vector>
#include "ParticleFX.h"
#include <boost/multi_index/detail/adl_swap.hpp>
#include <deque>

/**
 * \brief Обёртка для OpenGL VAO
 */
class VAO
{
private:
	GLuint vao;
	GLuint indices = 0;
	size_t cIndices = 0;
	std::vector<GLuint> mBuffers;
	GLuint genBuffer();
public:
	VAO();
	~VAO();
	VAO(const VAO&) = delete;
	void draw(GLenum mode = GL_TRIANGLES);
	void drawInstanced(GLenum mode, GLsizei count);

	class Loader
	{
	private:
		VAO& vao;
	public:
		Loader(VAO& v);
		~Loader();
		void store(GLuint attr, const std::vector<glm::vec2>& d, GLuint div = 0);
		void store(GLuint attr, const std::vector<glm::vec3>& d, GLuint div = 0);
		void store(GLuint attr, const std::vector<glm::vec4>& d, GLuint div = 0);
		void store(const std::vector<GLuint>& d);
	};
};