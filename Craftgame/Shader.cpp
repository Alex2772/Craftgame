#ifndef SERVER
#include "Shader.h"

#include "GameEngine.h"
#include <vector>

Shader::Shader(std::string vertex, std::string fragment) {
	
	Shader::vertex = glCreateShader(GL_VERTEX_SHADER);
	Shader::fragment = glCreateShader(GL_FRAGMENT_SHADER);

	CGE::instance->logger->info("Loading vertex shader " + vertex);
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	CGE::instance->logger->info("Loading fragment shader " + fragment);
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment, std::ios::in);
	if (FragmentShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	CGE::instance->logger->info("Compiling vertex shader " + vertex);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(Shader::vertex, 1, &VertexSourcePointer, NULL);
	glCompileShader(Shader::vertex);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetShaderiv(Shader::vertex, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(Shader::vertex, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (!Result) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(Shader::vertex, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		CGE::instance->logger->err(&VertexShaderErrorMessage[0]);
	}


	CGE::instance->logger->info("Compiling fragment shader " + fragment);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(Shader::fragment, 1, &FragmentSourcePointer, NULL);
	glCompileShader(Shader::fragment);

	glGetShaderiv(Shader::fragment, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(Shader::fragment, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (!Result) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(Shader::fragment, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		CGE::instance->logger->err(&FragmentShaderErrorMessage[0]);
	}

	Shader::program = glCreateProgram();
	glAttachShader(Shader::program, Shader::vertex);
	glAttachShader(Shader::program, Shader::fragment);
}
Shader::~Shader() {
	glDetachShader(Shader::program, Shader::vertex);
	glDetachShader(Shader::program, Shader::fragment);
	glDeleteShader(Shader::vertex);
	glDeleteShader(Shader::fragment);
	glDeleteProgram(Shader::program);
}
void Shader::link() {
	glLinkProgram(Shader::program);
}
void Shader::start() {
	glUseProgram(Shader::program);
}
void Shader::stop() {
	glUseProgram(0);
}
void Shader::bindAttribute(std::string name) {
	glBindAttribLocation(program, attributes, name.c_str());
	attributes++;
}

void Shader::loadFloat(size_t location, GLfloat value) {
	glUniform1f(location, value);
}
void Shader::loadVector(size_t location, glm::vec2 value) {
	glUniform2f(location, value.x, value.y);
}
void Shader::loadVector(size_t location, glm::vec3 value) {
	glUniform3f(location, value.x, value.y, value.z);
}
void Shader::loadVector(size_t location, glm::vec4 value) {
	glUniform4f(location, value.x, value.y, value.z, value.w);
}
void Shader::loadMatrix(size_t location, glm::mat4 value) {
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::loadInt(size_t location, int value) {
	glUniform1i(location, value);
}
void Shader::loadFloat(std::string location, GLfloat value) {
	glUniform1f(getUniform(location), value);
}
void Shader::loadVector(std::string location, glm::vec2 value) {
	glUniform2f(getUniform(location), value.x, value.y);
}
void Shader::loadVector(std::string location, glm::vec3 value) {
	glUniform3f(getUniform(location), value.x, value.y, value.z);
}
void Shader::loadVector(std::string location, glm::vec4 value) {
	glUniform4f(getUniform(location), value.x, value.y, value.z, value.w);
}
void Shader::loadMatrix(std::string location, glm::mat4 value) {
	glUniformMatrix4fv(getUniform(location), 1, GL_FALSE, &value[0][0]);
}
void Shader::loadInt(std::string location, int value) {
	glUniform1i(getUniform(location), value);
}
size_t Shader::getUniform(std::string name) {
	return glGetUniformLocation(Shader::program, name.c_str());
}

void Shader::loadArray(std::string name, std::vector<float>& vec) {
	for (int i = 0; i < vec.size(); i++) {
		loadFloat(getUniform(name + std::string("[") + std::to_string(i) + std::string("]")), vec[i]);
	}
}
void Shader::loadArray(std::string name, std::vector<glm::vec4>& vec) {
	for (int i = 0; i < vec.size(); i++) {
		loadVector(getUniform(name + std::string("[") + std::to_string(i) + std::string("]")), vec[i]);
	}
}
#endif