#include "AdvancedModel.h"
#include "ModelRegistry.h"
#include "GameEngine.h"
#include "BinaryInputStream.h"
#include "VAOHelper.h"
#include "Utils.h"
#include "GuiScreenLoad.h"

using namespace std;

Vertex::Vertex() {}
bool Vertex::isSet() {
	return texIndex != -1 && normalIndex != -1;
}

ModelRegistry::ModelRegistry() {
	models = new std::vector < AdvancedModel* >;
}
void ModelRegistry::registerModel(AdvancedModel* model) {
	models->push_back(model);
	CGE::instance->logger->info("Registered model " + model->resource.full);
}
AdvancedModel* ModelRegistry::getModel(_R resource) {
	static AdvancedModel* s = NULL;
	if (s != NULL && s->resource.full == resource.full)
		return s;
	for (std::vector<AdvancedModel*>::iterator i = models->begin(); i != models->end(); i++) {
		if ((*i)->resource.full == resource.full) {
			s = *i;
			return *i;
		}
	}
	if (models->empty())
		return nullptr;
	AdvancedModel* model = (*models)[0];
	CGE::instance->logger->warn("Accessed unknown model (" + resource.full + "), returning first item (" + model->resource.full + ")");
	return model;
}
#ifndef SERVER
Vertex* ModelRegistry::proccessVertex(int vertIndex, int texIndex, int normalIndex, vector<Vertex*>* vertices, vector<GLuint>* indices) {
	Vertex* currentVertex = ((*vertices)[vertIndex]);
	if (!currentVertex->isSet()) {
		currentVertex->texIndex = texIndex;
		currentVertex->normalIndex = normalIndex;
		indices->push_back(vertIndex);
	}
	else if (currentVertex->texIndex == texIndex && currentVertex->normalIndex == normalIndex)
		indices->push_back(vertIndex);
	else {
		Vertex* v = new Vertex;
		v->pos = currentVertex->pos;
		v->texIndex = texIndex;
		v->normalIndex = normalIndex;
		vertices->push_back(v);
		indices->push_back((unsigned int)(vertices->size()) - 1);
		return v;
	}
	return currentVertex;
}
void ModelRegistry::processTangent(Vertex* v0, Vertex* v1, Vertex* v2, vector<glm::vec2>* uv) {
	glm::vec3 deltaPos1 = glm::vec3(*v1->pos) - glm::vec3(*v0->pos);
	glm::vec3 deltaPos2 = glm::vec3(*v2->pos) - glm::vec3(*v0->pos);
	// UV delta
	glm::vec2 deltaUV1 = (*uv)[v1->texIndex] - (*uv)[v0->texIndex];
	glm::vec2 deltaUV2 = (*uv)[v2->texIndex] - (*uv)[v0->texIndex];

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	glm::vec3* t = new glm::vec3((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y));

	v0->tangent = t;
	v1->tangent = t;
	v2->tangent = t;
}

#endif

bool has_suffix(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
using namespace Assimp;
AdvancedModel* ModelRegistry::registerModel(std::string file, _R resource, bool useMaterials) {
	AdvancedModel* ggmodel = nullptr;
#ifndef SERVER
	CGE::instance->uiThread.push([&, file]()
	{
		if (!CGE::instance->guiScreens.empty()) {
			GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
			if (ls)
			{
				ls->setDisplayStringAsync(CGE::instance->currentLocale->localize("gui.load.asset", spair("asset", vr(file))));
			}
		}
	});
	if (has_suffix(file, std::string(".cgem"))) {
		AdvancedCGEMModel* model = new AdvancedCGEMModel(resource);
		ggmodel = model;
		BinaryInputStream in(file);
		if (!in.good()) {
			CGE::instance->logger->warn("Cannot load model " + resource.full + " from file " + file);
			return NULL;
		}
		model->comment = in.readString();
		ModelObject * o = NULL;
		vector<Vertex*> * vertices = NULL;
		vector<glm::vec2> * texCoords = NULL;
		vector<glm::vec3> * normal = NULL;
		vector<GLuint> * indices = NULL;
		while (in.good()) {
			byte head = in.read<byte>();
			if (head == (byte)0x05) {
				if (o != NULL && vertices != NULL) {
					o->vertices = new vector < glm::vec4 >;
					o->texCoords = new vector < glm::vec2 >;
					o->normal = new vector < glm::vec3 >;
					o->tangent = new vector < glm::vec3 >;
					for (int i = 0; i < vertices->size(); i++) {
						Vertex* v = (*vertices)[i];
						o->vertices->push_back(*v->pos);
						o->texCoords->push_back((*texCoords)[v->texIndex]);
						o->normal->push_back((*normal)[v->normalIndex]);
						o->tangent->push_back(*v->tangent);
					}
					o->indices = indices;
					delete vertices;
					delete texCoords;
					delete normal;
					model->objects.push_back(o);
				}
				o = new ModelObject();
				o->material = CGE::instance->materialRegistry->getMaterial(_R(in.readString()));

				vertices = new vector < Vertex* >;
				texCoords = new vector < glm::vec2 >;
				normal = new vector < glm::vec3 >;
				indices = new vector < GLuint >;
			}
			else if (head == (byte)0x01) {
				Vertex* v = new Vertex;
				float x = in.read<float>();
				float y = in.read<float>();
				float z = in.read<float>();
				v->pos = new glm::vec4(x, y, z, 1);
				vertices->push_back(v);
			}
			else if (head == (byte)0x02) {
				float x = in.read<float>();
				float y = in.read<float>();
				texCoords->push_back(glm::vec2(x, y));
			}
			else if (head == (byte)0x03) {
				float x = in.read<float>();
				float y = in.read<float>();
				float z = in.read<float>();
				normal->push_back(glm::vec3(x, y, z));
			}
			else if (head == (byte)0x04) {
				Vertex** vert = new Vertex*[3];
				for (int i = 0; i < 3; i++) {
					int index = in.read<int>() - 1;
					int texIndex = in.read<int>() - 1;
					int normals = in.read<int>() - 1;
					vert[i] = proccessVertex(index, texIndex, normals, vertices, indices);
				}
				processTangent(vert[0], vert[1], vert[2], texCoords);

				delete[] vert;
			}
			else {
				stringstream ss;
				ss << std::hex << (int)head << " caret " << in.tellg();
				CGE::instance->logger->warn("Unknown directive " + ss.str());
			}
		}
		o->vertices = new vector < glm::vec4 >;
		o->texCoords = new vector < glm::vec2 >;
		o->normal = new vector < glm::vec3 >;
		o->tangent = new vector < glm::vec3 >;
		for (int i = 0; i < vertices->size(); i++) {
			Vertex* v = (*vertices)[i];
			o->vertices->push_back(*v->pos);
			o->texCoords->push_back((*texCoords)[v->texIndex]);
			o->normal->push_back((*normal)[v->normalIndex]);
			o->tangent->push_back(*v->tangent);
		}
		o->indices = indices;
		delete vertices;
		delete texCoords;
		delete normal;
		model->objects.push_back(o);
		in.close();


		CGE::instance->uiThread.push([model]() {
			for (int i = 0; i < model->objects.size(); i++) {
				ModelObject* o = model->objects[i];
				o->vao = CGE::instance->vao->createVAO();
				CGE::instance->vao->storeData(0, 4, o->vertices);
				if (o->texCoords->size() > 0)
					CGE::instance->vao->storeData(1, 2, o->texCoords);
				CGE::instance->vao->storeData(2, 3, o->normal);
				CGE::instance->vao->storeData(3, 3, o->tangent);
				o->i = CGE::instance->vao->storeData(6, 1, o->indices);
			}
		});
	} else {
		std::string filename;
		{
			std::vector<std::string> tmp = Parsing::splitString(file, "/");
			filename = tmp[tmp.size() - 1];
			tmp = Parsing::splitString(filename, ".");
			filename = filename.substr(0, filename.length() - tmp[tmp.size() - 1].length() - 1);
		}
		AdvancedAssimpModel* m = new AdvancedAssimpModel(resource);
		ggmodel = m;
		ModelRegistry* t = this;
		m->importer = new Importer;
		m->mScene = m->importer->ReadFile(file, aiProcess_Triangulate |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices
			);
		m->m_GlobalInverseTransform = glm::inverse(
			Utils::convert(m->mScene->mRootNode->mTransformation)
		)
		;
		m->boneMapping = new std::map<std::string,AssimpBoneInfo>;
		
		for (size_t i = 0; i < m->mScene->mNumAnimations; i++) {
			aiAnimation* a = m->mScene->mAnimations[i];
			if (a->mName.length) {
				std::string ss = string(a->mName.C_Str());
				std::vector<std::string> strs = Parsing::splitString(ss, "|");
				ss = strs[strs.size() - 1];
				m->mAnimations[ss] = a;
			}
		}

		for (size_t i = 0; i < m->mScene->mNumMaterials; i++) {
			aiMaterial* material = m->mScene->mMaterials[i];
			std::string mname;
			{
				aiString name;
				material->Get(AI_MATKEY_NAME, name);
				mname = name.C_Str();
			}
			Material* fndMaterial = CGE::instance->materialRegistry->getMaterial(_R(std::string("craftgame:") + mname));
			if (!fndMaterial) {
				fndMaterial = new Material(_R(std::string("craftgame:model/") + filename + "/" + mname));
				// Цвет
				{
					aiColor3D color = aiColor3D(1, 1, 1);
					material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
					float alpha = 1.f;
					material->Get(AI_MATKEY_OPACITY, alpha);
					fndMaterial->setColor(glm::vec4(color.r, color.g, color.b, alpha));
				}
				// Текстурки
				{
					aiTextureType tt[] = {
						aiTextureType::aiTextureType_DIFFUSE,
						aiTextureType::aiTextureType_NORMALS
					};
					GLuint ids[sizeof(tt)];
					for (size_t i = 0; i < sizeof(tt) / sizeof(aiTextureType); i++) {
						aiString t;
						
						if (material->Get(AI_MATKEY_TEXTURE(tt[i], 0), t) == AI_SUCCESS && t.length) {
							std::string fname;
							std::string fpath(t.C_Str());
							std::vector<std::string> path = Parsing::splitString(fpath, "\\");
							fname = path[path.size() - 1];
							path = Parsing::splitString(fname, ".");
							if (path.size() > 1) {
								fname = fname.substr(0, fname.length() - path[path.size() - 1].length() - 1);
							}
							ids[i] = CGE::instance->textureManager->loadTexture(std::string("res/materials/") + fname +".png", true);
						}
						else
							ids[i] = 0;
					}
					fndMaterial->setTexture(ids[0]);
					fndMaterial->setNormalMap(ids[1]);
				}
				CGE::instance->materialRegistry->registerMaterial(fndMaterial);
			}
			m->mMaterials.push_back(fndMaterial);
		}

		size_t counter = 0;
		for (size_t i = 0; i < m->mScene->mNumMeshes; i++) {
			aiMesh* mesh = m->mScene->mMeshes[i];
			if ((mesh->HasTextureCoords(0) && mesh->HasNormals() && mesh->HasTangentsAndBitangents())) {
				AssimpModelObject * o = new AssimpModelObject;
				o->material = nullptr;
				o->vertices = new std::vector<glm::vec4>;
				o->texCoords = new std::vector<glm::vec2>;
				o->normal = new std::vector<glm::vec3>;
				o->tangent = new std::vector<glm::vec3>;
				o->indices = new std::vector<GLuint>;
				o->bones = new std::vector<AssimpBone>;
				o->name = mesh->mName.C_Str();
				if (o->name.empty())
				{
					o->name = std::to_string(i);
				}
				o->vertices->reserve(mesh->mNumVertices);
				o->texCoords->reserve(mesh->mNumVertices);
				o->normal->reserve(mesh->mNumVertices);
				o->tangent->reserve(mesh->mNumVertices);
				o->bones->resize(mesh->mNumVertices);
				memset(&((*o->bones)[0]), 0, sizeof(AssimpBone) * o->bones->size());

				o->material = m->mMaterials[mesh->mMaterialIndex];

				for (size_t j = 0; j < mesh->mNumFaces; j++) {
					const aiFace& face = mesh->mFaces[j];

					for (size_t k = 0; k < face.mNumIndices; k++) {
						o->indices->push_back(face.mIndices[k]);
					}
				}
				for (size_t j = 0; j < mesh->mNumBones; j++) {
					aiBone* bone = mesh->mBones[j];
					std::map<std::string, AssimpBoneInfo>::iterator mEntry = m->boneMapping->find(bone->mName.C_Str());
					for (size_t k = 0; k < bone->mNumWeights; k++) {
						aiVertexWeight w = bone->mWeights[k];
						if (w.mWeight > 0.15f) {
							unsigned int* ids = (*o->bones)[w.mVertexId].IDs;
							float* weights = (*o->bones)[w.mVertexId].weights;
							bool ok = false;
							for (size_t i = 0; i < BONES_PER_VERTEX; i++) {
								if (weights[i] == 0.f) {
									ids[i] = mEntry == m->boneMapping->end() ? counter : mEntry->second.index;
									weights[i] = w.mWeight;
									ok = true;
									break;
								}
							}
							if (!ok)
							{
								WARN(std::string("[ModelLoader] Detected mesh with more than ") + std::to_string(BONES_PER_VERTEX) + " bones per vertex; CGE couldn't handle that. " + file + ":" + mesh->mName.C_Str());
							}
						}
					}
					if (mEntry == m->boneMapping->end()) {
						AssimpBoneInfo i;
						i.mTransform = Utils::convert(bone->mOffsetMatrix);
						i.index = counter;
						counter++;
						(*m->boneMapping)[std::string(bone->mName.C_Str())] = i;
					}
				}
				for (size_t j = 0; j < mesh->mNumVertices; j++) {
					glm::vec4 v(Utils::convert(mesh->mVertices[j]), 1);
					o->vertices->push_back(v);
					o->normal->push_back(Utils::convert(mesh->mNormals[j]));
					o->tangent->push_back(Utils::convert(mesh->mTangents[j]));
					glm::vec3 temp = Utils::convert(mesh->mTextureCoords[0][j]);
					o->texCoords->push_back(glm::vec2(temp.x, 1.f - temp.y));
				}
				m->mObjects.push_back(o);
			} else {
				WARN(std::string(filename) + ": UVs, normals and tangents are needed in model. Mesh " + mesh->mName.C_Str());
			}
		}
		CGE::instance->uiThread.push([m]() {
			for (int i = 0; i < m->mObjects.size(); i++) {
				AssimpModelObject* o = m->mObjects[i];
				o->vao = CGE::instance->vao->createVAO();
				CGE::instance->vao->storeData(0, 4, o->vertices);
				CGE::instance->vao->storeData(1, 2, o->texCoords);
				CGE::instance->vao->storeData(2, 3, o->normal);
				CGE::instance->vao->storeData(3, 3, o->tangent);
				if (o->bones->size()) {
					GLuint bone;
					glGenBuffers(1, &bone);
					glBindBuffer(GL_ARRAY_BUFFER, bone);
					glBufferData(GL_ARRAY_BUFFER, sizeof(AssimpBone) * o->bones->size(), &((*o->bones)[0]), GL_STATIC_DRAW);
					glVertexAttribIPointer(4, 4, GL_INT, sizeof(AssimpBone), (const GLvoid*)0);
					glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(AssimpBone), (const GLvoid*)(sizeof(unsigned int) * BONES_PER_VERTEX));
					glVertexAttribIPointer(6, 4, GL_INT, sizeof(AssimpBone), (const GLvoid*)(sizeof(unsigned int) * BONES_PER_VERTEX / 2));
					glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(AssimpBone), (const GLvoid*)(sizeof(unsigned int) * BONES_PER_VERTEX + sizeof(float) * BONES_PER_VERTEX / 2));
					glEnableVertexAttribArray(0);
					glEnableVertexAttribArray(1);
					glEnableVertexAttribArray(2);
					glEnableVertexAttribArray(3);
					glEnableVertexAttribArray(4);
					glEnableVertexAttribArray(5);
					glEnableVertexAttribArray(6);
					glEnableVertexAttribArray(7);
				}
				o->i = CGE::instance->vao->storeData(6, 1, o->indices);
			}
		});
	}
	registerModel(ggmodel);
#endif
	return ggmodel;
}
