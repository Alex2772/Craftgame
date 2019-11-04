#pragma once

#include "gl.h"
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <vector>
#include <functional>
class Shader {
private:
	std::map<std::string, size_t> uniformLocations;
protected:
	size_t program = 0;
	size_t vertex = 0;
	size_t fragment = 0;
	size_t geometry = 0;
	size_t attributes = 0;
	std::string fvertex, ffragment;
	void unload();
	std::vector<std::string> mAttributes;
	GLuint createShader(std::string code, GLenum type);
public:
	std::string fgeometry;
	std::function<void()> onUpload;
	std::function<void()> onCompile;
	std::map<std::string, int> defines;
	glm::mat4 transform;
	void shaderCrashed(const std::string& shader, const char* error);
	Shader(std::string vertex, std::string fragment);
	~Shader();
	virtual void compile();
	void bindAttribute(std::string name);
	void link();
	virtual void start();
	virtual void stop();
	void loadFloat(size_t location, GLfloat value);
	void loadVector(size_t location, glm::vec2 value);
	void loadVector(size_t location, glm::vec3 value);
	void loadVector(size_t location, glm::vec4 value);
	void loadMatrix(size_t location, glm::mat4 value);
	void loadInt(size_t location, int value);
	void loadFloat(std::string location, GLfloat value);
	void loadVector(std::string location, glm::vec2 value);
	void loadVector(std::string location, glm::vec3 value);
	void loadVector(std::string location, glm::vec4 value);
	void loadMatrix(std::string location, glm::mat4 value);
	void loadInt(std::string location, int value);
	void loadArray(std::string name, std::vector<float> vec);
	void loadArray(std::string name, std::vector<glm::vec3> vec);
	void loadArray(std::string name, std::vector<glm::vec4> vec);

	void use();
	virtual void startNonG();

	size_t getUniform(const std::string& uniform);
};
