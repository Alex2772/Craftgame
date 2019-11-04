#pragma once

#include <glm/glm.hpp>
#include "AABB.h"

struct Plane
{
	float distanceToPoint(const glm::vec3& point) const;

	float distanceToOrigin;
	glm::vec3 normal;
};

class ViewFrustum
{
public:
	void update(const glm::mat4& projViewMatrix) noexcept;

	bool isBoxInFrustum(const AABB& box) const noexcept;
	bool isSphereInFrustum(const glm::vec3& pos, const float& radius) const noexcept;
private:
	Plane m_planes[6];
};