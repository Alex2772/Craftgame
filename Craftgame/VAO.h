#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <vector>

class VAO {
public:
	VAO() {}
	GLuint createVAO() {
		GLuint id;
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);
		return id;
	}
	GLuint storeData(GLuint attribute, GLuint size, std::vector<glm::vec3> * data) {
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, data->size() * sizeof(GLfloat) * 3, &(*data)[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(attribute, size, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return buffer;
	}
	GLuint storeData(GLuint attribute, GLuint size, std::vector<glm::vec2> * data) {
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, data->size() * sizeof(GLfloat) * 2, &(*data)[0].x, GL_STATIC_DRAW);
		glVertexAttribPointer(attribute, size, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return buffer;
	}
	GLuint storeData(GLuint attribute, GLuint size, std::vector<GLint> * data) {
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, data->size() * sizeof(GLint), &(*data)[0], GL_STATIC_DRAW);
		if (attribute >= 0)
			glVertexAttribPointer(attribute, size, GL_INT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return buffer;
	}
	GLuint storeData(GLuint attribute, GLuint size, std::vector<GLuint> * data) {
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->size() * sizeof(GLuint), &(*data)[0], GL_STATIC_DRAW);
		return buffer;
	}
};