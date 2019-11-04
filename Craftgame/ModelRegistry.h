#pragma once
#include "AdvancedModel.h"

class Vertex {
public:
	Vertex();
	glm::vec4* pos = NULL;
	int texIndex = -1;
	int normalIndex = -1;
	bool isSet();
	glm::vec3* tangent = NULL;
};

class ModelRegistry {
private:
	std::vector<AdvancedModel*>* models;
#ifndef SERVER
	Vertex* proccessVertex(int vertIndex, int texIndex, int normalIndex, std::vector<Vertex*>* vertices, std::vector<GLuint>* indices);
	void processTangent(Vertex* v0, Vertex* v1, Vertex* v2, std::vector<glm::vec2>* uv);
#endif
public:
	ModelRegistry();
	void registerModel(AdvancedModel* model);
	AdvancedModel* getModel(_R resource);
	AdvancedModel* registerModel(std::string file, _R resource, bool useMaterials = true);
};
