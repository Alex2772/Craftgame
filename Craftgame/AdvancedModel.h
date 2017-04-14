#pragma once
#include <vector>
#include "Material.h"
#include <glm/glm.hpp>

class ModelObject {
public:
	ModelObject();
	Material* material;
	std::vector<glm::vec3>* vertices;
	std::vector<glm::vec2>* texCoords;
	std::vector<glm::vec3>* normal;
	std::vector<glm::vec3>* tangent;
	std::vector<GLuint>* indices;
	GLuint vao;
	GLuint i;
};

class AdvancedModel {
public:
	AdvancedModel(_R resource);
	~AdvancedModel();
	_R resource;
	std::vector<ModelObject*> objects;
	std::string comment;
};