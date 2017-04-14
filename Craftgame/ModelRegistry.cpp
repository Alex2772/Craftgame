#ifndef SERVER
#include "AdvancedModel.h"
#include "ModelRegistry.h"
#include "GameEngine.h"
#include "BinaryInputStream.h"
#include "VAO.h"
#include <fbxsdk.h>

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
AdvancedModel* ModelRegistry::getModel(_R& resource) {
	static AdvancedModel* s = NULL;
	if (s != NULL && s->resource.full == resource.full)
		return s;
	for (std::vector<AdvancedModel*>::iterator i = models->begin(); i != models->end(); i++) {
		if ((*i)->resource.full == resource.full) {
			s = *i;
			return *i;
		}
	}
	AdvancedModel* model = (*models)[0];
	CGE::instance->logger->warn("Accessed unknown model (" + resource.full + "), returning first item (" + model->resource.full + ")");
	return model;
}
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
		indices->push_back(unsigned int(vertices->size()) - 1);
		return v;
	}
	return currentVertex;
}
void ModelRegistry::processTangent(Vertex* v0, Vertex* v1, Vertex* v2, vector<glm::vec2>* uv) {
	glm::vec3 deltaPos1 = (*v1->pos) - (*v0->pos);
	glm::vec3 deltaPos2 = (*v2->pos) - (*v0->pos);
	// UV delta
	glm::vec2 deltaUV1 = (*uv)[v1->texIndex] - (*uv)[v0->texIndex];
	glm::vec2 deltaUV2 = (*uv)[v2->texIndex] - (*uv)[v0->texIndex];

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	glm::vec3* t = new glm::vec3((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y));

	v0->tangent = t;
	v1->tangent = t;
	v2->tangent = t;
}

void recursiveFbxScan(AdvancedModel* model, FbxNode* node, bool useMaterials) {
	int numChilds = node->GetChildCount();
	FbxNode* child = nullptr;

	for (int i = 0; i < numChilds; i++) {
		child = node->GetChild(i);

		if (child->GetNodeAttribute() == nullptr)
			continue;

		FbxNodeAttribute::EType AttributeType = child->GetNodeAttribute()->GetAttributeType();
		switch (AttributeType) {
		case FbxNodeAttribute::eMesh: {
			FbxMesh* mesh = child->GetMesh();

			ModelObject* m = new ModelObject;
			m->vertices = new std::vector<glm::vec3>;
			m->texCoords = new std::vector<glm::vec2>;
			m->normal = new std::vector<glm::vec3>;
			m->indices = new std::vector<GLuint>;
			m->tangent = new std::vector<glm::vec3>;
			m->material = CGE::instance->materialRegistry->getMaterial(useMaterials ? _R(model->resource.full + "/" + child->GetName()) : _R("craftgame:notexture"));

			FbxGeometryElementNormal* normal = mesh->GetElementNormal();
			if (!normal)
				throw  CraftgameException("CGE requires normals on models");
			FbxGeometryElementUV* uv = mesh->GetElementUV();

			if (!uv)
				throw  CraftgameException("CGE requires uv on models");

			/* CP */
			std::vector<glm::vec3> mControlPoints;
			for (int i = 0; i < mesh->GetControlPointsCount(); i++) {
				FbxVector4 d = mesh->GetControlPointAt(i);
				glm::vec3 v;
				v.x = d.mData[0];
				v.y = d.mData[1];
				v.z = d.mData[2];
				mControlPoints.push_back(v);
			}

			for (int i = 0; i < mesh->GetPolygonCount(); i++) {
				int size = mesh->GetPolygonSize(i);
				if (size != 3)
					throw  CraftgameException(("Unsupported " + std::to_string(size) + " vertices polygon; use triangles").c_str());
				for (int j = 0; j < size; j++) {
					int ctrlPointIndex = mesh->GetPolygonVertex(i, j);
					size_t vertex = i * 3 + j;


					glm::vec3 outNormal;
					/* Normal mapping */
					switch (normal->GetMappingMode()) {
					case FbxGeometryElement::eByControlPoint:
						switch (normal->GetReferenceMode()) {
						case FbxGeometryElement::eDirect: {
							FbxVector4 s = normal->GetDirectArray().GetAt(ctrlPointIndex);
							outNormal.x = static_cast<float>(s.mData[0]);
							outNormal.y = static_cast<float>(s.mData[1]);
							outNormal.z = static_cast<float>(s.mData[2]);
							break;
						}
						case FbxGeometryElement::eIndexToDirect: {
							FbxVector4 s = normal->GetDirectArray().GetAt(normal->GetIndexArray().GetAt(ctrlPointIndex));
							outNormal.x = static_cast<float>(s.mData[0]);
							outNormal.y = static_cast<float>(s.mData[1]);
							outNormal.z = static_cast<float>(s.mData[2]);
							break;
						}
						default:
							throw  CraftgameException("Invalid reference");
						}
						break;
					case FbxGeometryElement::eByPolygonVertex:
						switch (normal->GetReferenceMode()) {
						case FbxGeometryElement::eDirect: {
							FbxVector4 s = normal->GetDirectArray().GetAt(vertex);
							outNormal.x = static_cast<float>(s.mData[0]);
							outNormal.y = static_cast<float>(s.mData[1]);
							outNormal.z = static_cast<float>(s.mData[2]);
							break;
						}
						case FbxGeometryElement::eIndexToDirect: {
							FbxVector4 s = normal->GetDirectArray().GetAt(normal->GetIndexArray().GetAt(vertex));
							outNormal.x = static_cast<float>(s.mData[0]);
							outNormal.y = static_cast<float>(s.mData[1]);
							outNormal.z = static_cast<float>(s.mData[2]);
							break;
						}
						default:
							throw  CraftgameException("Invalid reference");
						}
						break;
					default:
						throw  CraftgameException("Unsupported mapping mode");
					}
					m->normal->push_back(outNormal);

					/* UV */
					glm::vec2 outUV;
					switch (uv->GetMappingMode()) {
					case FbxGeometryElement::eByControlPoint:
						switch (uv->GetReferenceMode()) {
						case FbxGeometryElement::eDirect: {
							FbxVector2 s = uv->GetDirectArray().GetAt(ctrlPointIndex);
							outUV.x = static_cast<float>(s.mData[0]);
							outUV.y = 1.f - static_cast<float>(s.mData[1]);
							break;
						}
						case FbxGeometryElement::eIndexToDirect: {
							FbxVector2 s = uv->GetDirectArray().GetAt(uv->GetIndexArray().GetAt(ctrlPointIndex));
							outUV.x = static_cast<float>(s.mData[0]);
							outUV.y = 1.f - static_cast<float>(s.mData[1]);
							break;
						}
						default:
							throw  CraftgameException("Invalid reference");
						}
						break;
					case FbxGeometryElement::eByPolygonVertex:
						switch (uv->GetReferenceMode()) {
						case FbxGeometryElement::eDirect: {
							FbxVector2 s = uv->GetDirectArray().GetAt(vertex);
							outUV.x = static_cast<float>(s.mData[0]);
							outUV.y = 1.f - static_cast<float>(s.mData[1]);
							break;
						}
						case FbxGeometryElement::eIndexToDirect: {
							FbxVector2 s = uv->GetDirectArray().GetAt(uv->GetIndexArray().GetAt(vertex));
							outUV.x = static_cast<float>(s.mData[0]);
							outUV.y = 1.f - static_cast<float>(s.mData[1]);
							break;
						}
						default:
							throw  CraftgameException("Invalid reference");
						}
						break;
					default:
						throw  CraftgameException("Unsupported mapping mode");
					}



					m->vertices->push_back(mControlPoints[ctrlPointIndex]);
					m->texCoords->push_back(outUV);
					m->indices->push_back(i * 3 + j);

					/*
					FbxVector4 v  = normal->GetDirectArray().GetAt(i * 3 + j);
					glm::vec3 d;
					d.x = v.mData[0];
					d.y = v.mData[1];
					d.z = v.mData[2];
					m->normal->push_back(d);*/
				}
				unsigned int v0 = m->vertices->size() - 3;
				unsigned int v1 = m->vertices->size() - 2;
				unsigned int v2 = m->vertices->size() - 1;
				glm::vec3 deltaPos1 = (*m->vertices)[v1] - (*m->vertices)[v0];
				glm::vec3 deltaPos2 = (*m->vertices)[v2] - (*m->vertices)[v1];
				// UV delta
				glm::vec2 deltaUV1 = (*m->texCoords)[v1] - (*m->texCoords)[v0];
				glm::vec2 deltaUV2 = (*m->texCoords)[v2] - (*m->texCoords)[v0];

				float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				glm::vec3 t((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y));
				for (int i = 0; i < 3; i++)
					m->tangent->push_back(t);
			}
			model->objects.push_back(m);
			break;
		}
		case FbxNodeAttribute::eSkeleton: {
			
			break;
		}
		}

		recursiveFbxScan(model, child, useMaterials);
	}
}
bool has_suffix(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
AdvancedModel* ModelRegistry::registerModel(std::string file, _R resource, bool useMaterials) {
	AdvancedModel* model = new AdvancedModel(resource);
	if (has_suffix(file, std::string(".cgem"))) {
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
					o->vertices = new vector < glm::vec3 >;
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
				v->pos = new glm::vec3(x, y, z);
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
		o->vertices = new vector < glm::vec3 >;
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
	}
	else if (has_suffix(file, std::string(".fbx"))) {
		FbxManager* manager = FbxManager::Create();
		FbxIOSettings* io = FbxIOSettings::Create(manager, IOSROOT);
		manager->SetIOSettings(io);

		FbxImporter* importer = FbxImporter::Create(manager, "");
		importer->Initialize(file.c_str(), -1, manager->GetIOSettings());

		FbxScene* scene = FbxScene::Create(manager, "temp");
		importer->Import(scene);
		importer->Destroy();

		FbxNode* root = scene->GetRootNode();

		if (root) {
			recursiveFbxScan(model, root, useMaterials);
		}

		/* Processing joints and skeletonal data */

	}
	for (int i = 0; i < model->objects.size(); i++) {
		ModelObject* o = model->objects[i];
		o->vao = CGE::instance->vao->createVAO();
		CGE::instance->vao->storeData(0, 3, o->vertices);
		if (o->texCoords->size() > 0)
			CGE::instance->vao->storeData(1, 2, o->texCoords);
		CGE::instance->vao->storeData(2, 3, o->normal);
		CGE::instance->vao->storeData(3, 3, o->tangent);
		o->i = CGE::instance->vao->storeData(5, 1, o->indices);
	}
	registerModel(model);
	return model;
}
#endif