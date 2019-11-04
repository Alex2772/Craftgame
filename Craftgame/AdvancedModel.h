#pragma once
#include <vector>
#include "Material.h"
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#define BONES_PER_VERTEX 8

class ModelObject {
public:
	ModelObject();
	Material* material;
#ifndef SERVER
	std::vector<glm::vec4>* vertices;
	std::vector<glm::vec2>* texCoords;
	std::vector<glm::vec3>* normal;
	std::vector<glm::vec3>* tangent;
	std::vector<GLuint>* indices;
	GLuint vao;
	GLuint i;
#endif
};

class AdvancedModel {
public:
	AdvancedModel(_R resource);
	virtual ~AdvancedModel();
	_R resource;
};

class AdvancedCGEMModel: public AdvancedModel {
public:
	AdvancedCGEMModel(_R resource);
	~AdvancedCGEMModel();
	std::vector<ModelObject*> objects;
	std::string comment;
};
struct AssimpBone {
	unsigned int IDs[BONES_PER_VERTEX];
	float weights[BONES_PER_VERTEX];
};
struct AssimpBoneInfo {
	glm::mat4 mTransform;
	glm::mat4 mFinalTransform;
	glm::mat4 mOffset;
	size_t index;
};
class AssimpModelObject {
public:
	AssimpModelObject();
	std::string name;
	Material* material = nullptr;
	std::vector<AssimpBone>* bones;
#ifndef SERVER
	std::vector<glm::vec4>* vertices;
	std::vector<glm::vec2>* texCoords;
	std::vector<glm::vec3>* normal;
	std::vector<glm::vec3>* tangent;
	std::vector<GLuint>* indices;
	GLuint vao;
	GLuint i;
#endif
};

struct AssimpAnimation {
	aiAnimation* handle;
	float time;
	float value;
};

struct SkeletalAnimation {
	unsigned char state = 0;
	float time = 0;
	float value = 0;
	float speed = 1.f;
};

struct BoneTransform
{
	glm::mat4 m;
	glm::mat4 o;
	std::string name;
};

class AdvancedAssimpModel : public AdvancedModel {
private:
	aiNodeAnim* findNodeAnim(AssimpAnimation& a, std::string name);
	std::map<std::string, SkeletalAnimation>* mPAnims = nullptr;
public:
	AdvancedAssimpModel(_R resource);
	~AdvancedAssimpModel();
	void nextFrame();
	std::vector<AssimpModelObject*> mObjects;
	glm::mat4 m_GlobalInverseTransform;
	std::map<std::string, aiAnimation*> mAnimations;
	void boneTransform(std::vector<BoneTransform>& m, std::map<std::string, SkeletalAnimation>* anims, std::map<std::string, glm::mat4>& at);
	void recursiveNodeScan(std::vector<AssimpAnimation>& animation, aiNode* node, glm::mat4& parent, std::map<std::string, glm::mat4>& at);
	std::map<std::string, AssimpBoneInfo>* boneMapping;
	const aiScene* mScene;
	Assimp::Importer* importer;
	std::vector<Material*> mMaterials;
};
