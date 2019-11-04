#include "AABB.h"

glm::vec3 AABB::getPos() const {
	return (min + max) / glm::vec3(2.f);
}

glm::vec3 AABB::getDimensions() const
{
	return glm::abs(max - min) / 2.f;
}

bool AABB::test(const AABB& aabb) const
{
	glm::vec3 c1 = getPos();
	glm::vec3 c2 = aabb.getPos();
	
	if (glm::abs(c1.x - c2.x) > (getWidthX() / 2 + aabb.getWidthX() / 2))
		return false;
	if (glm::abs(c1.z - c2.z) > (getWidthZ() / 2 + aabb.getWidthZ() / 2))
		return false;
	if (glm::abs(c1.y - c2.y) > (getHeight() / 2 + aabb.getHeight() / 2))
		return false;

	return true;
}

glm::vec3 AABB::getNearestPointTo(glm::vec3 point)
{
	return glm::clamp(point, min, max);
}
