#ifndef SERVER
#include "AdvancedModel.h"
#include "Utils.h"
#include "GameEngine.h"

ModelObject::ModelObject() {}

AdvancedModel::AdvancedModel(_R _r) :
	resource(_r) {

}
AdvancedModel::~AdvancedModel() {
}

AdvancedCGEMModel::AdvancedCGEMModel(_R resource):
	AdvancedModel(resource)
{
}

AdvancedCGEMModel::~AdvancedCGEMModel()
{
	for (int i = 0; i < objects.size(); i++) {
		delete objects[i];
	}
	objects.clear();
}


AssimpModelObject::AssimpModelObject() {}

aiNodeAnim * AdvancedAssimpModel::findNodeAnim(AssimpAnimation & a, std::string name)
{
	for (size_t i = 0; i < a.handle->mNumChannels; i++) {
		aiNodeAnim* node = a.handle->mChannels[i];
		if (std::string(node->mNodeName.C_Str()) == name) {
			return node;
		}
	}
	return nullptr;
}

AdvancedAssimpModel::AdvancedAssimpModel(_R resource) :
	AdvancedModel(resource)
{
}

AdvancedAssimpModel::~AdvancedAssimpModel()
{
	for (int i = 0; i < mObjects.size(); i++) {
		delete mObjects[i];
	}
	mObjects.clear();
}

void AdvancedAssimpModel::nextFrame()
{
	if (CGE::instance->renderer->renderType & GameRenderer::RENDER_FIRST) {
		assert(mPAnims);
		for (std::map<std::string, SkeletalAnimation>::iterator i = mPAnims->begin(); i != mPAnims->end(); i++) {
			AssimpAnimation a;
			a.handle = mAnimations[i->first];
			if (!a.handle)
				continue;
			bool repeating = i->second.state & 0x80;
			char val = i->second.state & 0x0f;
			switch (val) {
			case 1: {
				i->second.value += CGE::instance->millis * 5;
				i->second.value = glm::clamp(i->second.value, 0.f, 1.f);
				break;
			}
			case 0:
				i->second.value -= CGE::instance->millis * 4;
				i->second.value = glm::clamp(i->second.value, 0.f, 1.f);
				break;
			}
			if (repeating) {
				i->second.time = fmod(i->second.time + (CGE::instance->millis) * a.handle->mTicksPerSecond * i->second.speed, a.handle->mDuration);
			}
			else {
				i->second.time += (CGE::instance->millis) * a.handle->mTicksPerSecond * i->second.speed;
				if (i->second.time > a.handle->mDuration) {
					i->second.time = a.handle->mDuration;
					i->second.state &= 0x80;
				}
			}
		}
	}
}

void AdvancedAssimpModel::boneTransform(std::vector<BoneTransform>& m, std::map<std::string, SkeletalAnimation>* anims, std::map<std::string, glm::mat4>& at)
{
	mPAnims = anims;
	std::vector<AssimpAnimation> sAnims;
	for (std::map<std::string, SkeletalAnimation>::iterator i = anims->begin(); i != anims->end(); i++) {
		AssimpAnimation a;
		a.handle = mAnimations[i->first];
		if (!a.handle)
			continue;
		a.time = i->second.time;
		a.value = i->second.value;
		if (i->second.value > 0 && i->second.time != a.handle->mDuration) {
			sAnims.push_back(a);
		}
	}
	glm::mat4 id(1.0);
	recursiveNodeScan(sAnims, mScene->mRootNode, id, at);
	for (std::map<std::string, AssimpBoneInfo>::iterator j = boneMapping->begin(); j != boneMapping->end(); j++) {
		if (m.size() <= j->second.index) {
			m.reserve(j->second.index + 1);
			while (m.size() != m.capacity())
				m.push_back({ glm::mat4(1.0) });
		}
		m[j->second.index].m = j->second.mFinalTransform;
		m[j->second.index].o = j->second.mOffset;
		if (m[j->second.index].name.empty())
			m[j->second.index].name = j->first;
	}
	/*
	if (!m.empty())
		for (std::map<std::string, AssimpBoneInfo>::iterator j = boneMapping->begin(); j != boneMapping->end(); j++) {
			glm::mat4 f = m[j->second.index];
			m[j->second.index] = f;
		}*/
}
unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}
unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	return 0;
}

void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// для интерполирования требуется не менее 2 значений...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime -
		pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	return 0;
}

void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}
void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void AdvancedAssimpModel::recursiveNodeScan(std::vector<AssimpAnimation> & animation, aiNode * node, glm::mat4 & parent, std::map<std::string, glm::mat4>& at)
{
	std::map<std::string, AssimpBoneInfo>::iterator bi = boneMapping->find(node->mName.C_Str());
	glm::mat4 nodeTransform = Utils::convert(node->mTransformation);
	for (size_t i = 0; i < animation.size(); i++) {
		aiNodeAnim* pNodeAnim = findNodeAnim(animation[i], node->mName.C_Str());
		if (pNodeAnim) {
			glm::mat4 scaleM(1.0);
			glm::mat4 RotationM(1.0);
			glm::mat4 translation(1.0);
			// Интерполируем масштабирование и генерируем матрицу преобразования масштаба
			aiVector3D Scaling;
			CalcInterpolatedScaling(Scaling, animation[i].time, pNodeAnim);
			scaleM = glm::scale(glm::mat4(1.0), glm::vec3(Scaling.x, Scaling.y, Scaling.z));

			// Интерполируем вращение и генерируем матрицу вращения
			aiQuaternion RotationQ;
			CalcInterpolatedRotation(RotationQ, animation[i].time, pNodeAnim);
			RotationM = Utils::convert(aiMatrix4x4(RotationQ.GetMatrix()));

			//  Интерполируем смещение и генерируем матрицу смещения
			aiVector3D Translation;
			CalcInterpolatedPosition(Translation, animation[i].time, pNodeAnim);
			translation = glm::translate(glm::mat4(1.0) , glm::vec3(Translation.x, Translation.y, Translation.z));
			nodeTransform += ((translation * RotationM * scaleM)
				- Utils::convert(node->mTransformation)
				)
				* animation[i].value
			;
		}
	}
	glm::mat4 gt = parent * nodeTransform * at[node->mName.C_Str()];
	if (bi != boneMapping->end()) {
		bi->second.mFinalTransform = gt;
		bi->second.mOffset = bi->second.mTransform;
	}
	for (size_t i = 0; i < node->mNumChildren; i++) {
		recursiveNodeScan(animation, node->mChildren[i], gt, at);
	}
}
#endif