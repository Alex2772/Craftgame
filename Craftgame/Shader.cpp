#ifndef SERVER
#include "GuiScreenMessageDialog.h"
#include "Shader.h"
#include "Utils.h"
#include "GameEngine.h"
#include <vector>

std::string _loading_errors;

void Shader::shaderCrashed(const std::string & shader, const char * error)
{
	std::string err(error);
	INFO(std::string("Shader ") + shader + " crashed: " + error);
	_loading_errors += std::string("\n\n") + shader + ": " + err;
	assert(0);
}

Shader::Shader(std::string vertex, std::string fragment):
	fvertex(vertex),
	ffragment(fragment)
{
}
Shader::~Shader() {
	unload();
}
void Shader::unload() {
	uniformLocations.clear();
	glDetachShader(program, vertex);
	glDetachShader(program, fragment);
	if (geometry)
	{
		glDetachShader(program, geometry);
		glDeleteShader(geometry);
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteProgram(program);
}
//#include <boost/uuid/uuid.hpp>            // uuid class
//#include <boost/uuid/uuid_generators.hpp> // generators
//#include <boost/uuid/uuid_io.hpp>  
GLuint Shader::createShader(std::string code, GLenum type)
{
	GLuint shader = glCreateShader(type);

	std::string defin;

	for (std::pair<std::string, int> d : defines) {
		defin += std::string("#define ") + d.first + " " + std::to_string(d.second) + "\n";
	}
	code.insert(code.find('\n') + 1, defin);
	
	char const * VertexSourcePointer = code.c_str();

	/*
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	std::stringstream ss;
	ss << uuid;
	std::ofstream fos(ss.str());
	fos << code;
	fos.close();
	*/
	glShaderSource(shader, 1, &VertexSourcePointer, NULL);
	glCompileShader(shader);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (!Result) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		shaderCrashed(code, &VertexShaderErrorMessage[0]);
		CGE::instance->logger->err(&VertexShaderErrorMessage[0]);
	}
	return shader;
}

void Shader::compile()
{
	if (program != 0) {
		unload();
	}
	if (onCompile)
		onCompile();
	std::string t1;
	Utils::fileToString(fvertex, t1);
	vertex = createShader(t1, GL_VERTEX_SHADER);
	t1.clear();
	Utils::fileToString(ffragment, t1);
	fragment = createShader(t1, GL_FRAGMENT_SHADER);
	if (!fgeometry.empty())
	{
		t1.clear();
		Utils::fileToString(fgeometry, t1);
		geometry = createShader(t1, GL_GEOMETRY_SHADER);
	}
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	if (geometry)
		glAttachShader(program, geometry);
	if (!mAttributes.empty()) {
		size_t co = 0;
		for (std::string& s : mAttributes) {
			glBindAttribLocation(program, co++, s.c_str());
		}
	}
	if (!mAttributes.empty()) {
		link();
	}
}
void Shader::link() {
	assert(program != 0);
	glLinkProgram(Shader::program);
	if (onUpload) {
		start();
		onUpload();
	}
}
void Shader::start() {
	if (CGE::instance->currentShader == this)
		return;
	if (CGE::instance->currentShader)
		CGE::instance->currentShader->stop();
	glUseProgram(Shader::program);
	CGE::instance->currentShader = this;
}
void Shader::stop() {
	glUseProgram(0);
	CGE::instance->currentShader = nullptr;
}
void Shader::bindAttribute(std::string name) {
	glBindAttribLocation(program, attributes, name.c_str());
	attributes++;
	mAttributes.push_back(name);
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
size_t Shader::getUniform(const std::string& name) {
	auto it = uniformLocations.find(name);
	if (it == uniformLocations.end())
	{
		size_t loc = glGetUniformLocation(Shader::program, name.c_str());
		uniformLocations[name] = loc;
		return loc;
	}
	return it->second;
}

void Shader::loadArray(std::string name, std::vector<float> vec) {
	for (int i = 0; i < vec.size(); i++) {
		loadFloat(getUniform(name + std::string("[") + std::to_string(i) + std::string("]")), vec[i]);
	}
}
void Shader::loadArray(std::string name, std::vector<glm::vec4> vec) {
	for (int i = 0; i < vec.size(); i++) {
		loadVector(getUniform(name + std::string("[") + std::to_string(i) + std::string("]")), vec[i]);
	}
}

void Shader::use()
{
	glUseProgram(program);
}


void Shader::startNonG()
{
	glUseProgram(Shader::program);
	CGE::instance->currentShader = this;
}
#endif
