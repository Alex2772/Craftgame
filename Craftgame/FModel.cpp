#include "FModel.h"

FModel::FModel(std::string path) {
	FbxManager* manager = FbxManager::Create();
	FbxIOSettings* io = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(io);

	FbxImporter* importer = FbxImporter::Create(manager, "");
	importer->Initialize(path.c_str(), -1, manager->GetIOSettings());

	FbxScene* scene = FbxScene::Create(manager, "temp");
	importer->Import(scene);
	importer->Destroy();

	FbxNode* root = scene->GetRootNode();

	if (root)
		recursiveFbxScan(root);
}
void FModel::recursiveFbxScan(FbxNode* node) {
	int numChilds = node->GetChildCount();
	FbxNode* child = nullptr;
	
	for (int i = 0; i < numChilds; i++) {
		child = node->GetChild(i);
		FbxMesh* mesh = child->GetMesh();

		if (mesh != nullptr) {
			int numVerts = mesh->GetControlPointsCount();

			for (int j = 0; j < numVerts; j++) {
				FbxVector4 vector = mesh->GetControlPointAt(j);
				glm::vec3 vec;
				vec.x = (float)vector.mData[0];
				vec.y = (float)vector.mData[1];
				vec.z = (float)vector.mData[2];

				vertices.push_back(vec);
			}

			unsigned int *indices = (unsigned int*)mesh->GetPolygonVertices();
			
			for (int i = 0; i < numVerts; i++)
				FModel::indices.push_back(indices[i]);
			
			FbxGeometryElementUV* uv = mesh->GetElementUV();
			if (uv) {
				unsigned int vertexCounter = 0;
				for (int p = 0; p < mesh->GetPolygonCount(); p++) {
					for (int i = 0; i < 2; i++) {
						FbxVector2 d = uv->GetDirectArray().GetAt(vertexCounter);
						vertexCounter++;
						texCoords.push_back(glm::vec2((float)d.mData[0], (float)d.mData[1]));
					}
				}
			}
			FbxGeometryElementNormal* normal = mesh->GetElementNormal();
			if (normal) {
				unsigned int vertexCounter = 0;
				for (int p = 0; p < mesh->GetPolygonCount(); p++) {
					for (int i = 0; i < 3; i++) {
						FbxVector4 d = normal->GetDirectArray().GetAt(vertexCounter);
						vertexCounter++;
						FModel::normal.push_back(glm::vec3((float)d.mData[0], (float)d.mData[1], (float)d.mData[2]));
					}
				}
			}
		}
		recursiveFbxScan(child);
	}
}