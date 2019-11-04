#pragma once

#include <fbxsdk.h>
#include "StructuredModel.h"

class FbxStructuredModel : public StructuredModel {
private:
	FbxNode* root = nullptr;
public:
	FbxStructuredModel(FbxNode* root);
	~FbxStructuredModel();
	virtual void render();
};