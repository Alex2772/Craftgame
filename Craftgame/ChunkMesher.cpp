#include "ChunkMesher.h"


void CFace::addVertex(glm::vec3 pos, glm::vec2 u)
{
	position.push_back(pos);
	uv.push_back(u);
}

ChunkMesher::ChunkMesher()
{
	
}

glm::vec3 calcNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
	glm::vec3 u = v2 - v1;
	glm::vec3 v = v3 - v1;

	return glm::cross(u, v);
}


void ChunkMesher::addFace(CFace& f)
{
	assert(f.position.size() == 4);
	for (size_t i = 0; i < f.position.size(); i++)
		position.push_back(f.position[i]);

	for (size_t i = 0; i < f.uv.size(); i++)
		uv.push_back(f.uv[i]);
	GLuint i = position.size() - 4;
	indices.push_back(i);
	indices.push_back(i + 1);
	indices.push_back(i + 2);
	indices.push_back(i + 2);
	indices.push_back(i + 1);
	indices.push_back(i + 3);
	i = indices.size() - 6;
	for (size_t k = 0; k < 2; k++) {
		glm::vec3 v0 = position[indices[i]];
		glm::vec3 v1 = position[indices[i + 1]];
		glm::vec3 v2 = position[indices[i + 2]];
		glm::vec3 n = calcNormal(v0, v1, v2);
		for (unsigned char j = 0; j < 2; j++)
			normal.push_back(n);

		glm::vec2 uv0 = uv[indices[i++]];
		glm::vec2 uv1 = uv[indices[i++]];
		glm::vec2 uv2 = uv[indices[i++]];

		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;
		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 t = glm::vec3((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);
		for (unsigned char j = 0; j < 2; j++)
			tangent.push_back(t);
	}
}