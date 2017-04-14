#pragma once
#include <GL/glew.h>
#include <Windows.h>
#include <GL\GL.h>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <vector>

class Shader {
private:
	size_t program;
	size_t vertex;
	size_t fragment;
	size_t attributes = 0;
public:
	Shader(std::string vertex, std::string fragment);
	~Shader();
	void bindAttribute(std::string name);
	void link();
	void start();
	void stop();

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
	void loadArray(std::string name, std::vector<float>& vec);
	void loadArray(std::string name, std::vector<glm::vec3>& vec);
	void loadArray(std::string name, std::vector<glm::vec4>& vec);

	size_t getUniform(std::string uniform);
};
