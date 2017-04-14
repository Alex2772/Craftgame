#pragma once
#include "AdvancedModel.h"



using namespace std;

class Vertex {
public:
	Vertex();
	~Vertex();
	glm::vec3* pos = NULL;
	int texIndex = -1;
	int normalIndex = -1;
	bool isSet();
	glm::vec3* tangent = NULL;
};

class ModelRegistry {
private:
	std::vector<AdvancedModel*>* models;
	Vertex* proccessVertex(int vertIndex, int texIndex, int normalIndex, vector<Vertex*>* vertices, vector<GLuint>* indices);
	void processTangent(Vertex* v0, Vertex* v1, Vertex* v2, vector<glm::vec2>* uv);
public:
	ModelRegistry();
	void registerModel(AdvancedModel* model);
	AdvancedModel* getModel(_R& resource);
	AdvancedModel* registerModel(std::string file, _R resource, bool useMaterials = true);
};