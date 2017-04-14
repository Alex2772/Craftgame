#pragma once

#include <string>
#include <fbxsdk.h>
#include <vector>
#ifndef SERVER
#include <GL/glew.h>
#include <GL/GL.h>
#endif
#include <glm/glm.hpp>

class FModel {
private:
	std::string path;
	void recursiveFbxScan(FbxNode* node);
public:
	FModel(std::string path);
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normal;
	std::vector<size_t> indices;
};
