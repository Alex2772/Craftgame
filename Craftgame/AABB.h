#pragma once
#include <glm/glm.hpp>

class AABB
{
public:
	glm::vec3 min;
	glm::vec3 max;

	AABB() {}
	AABB(const AABB& c)
	{
		min = c.min;
		max = c.max;
	}
	AABB(glm::vec3 mi, glm::vec3 ma)
	{
		min = mi;
		max = ma;
	}

	/*
	bool isCollidesWith(AABB& r)
	{
		//return (min.x > r.min.x && r.min.x < max.x || );
	}*/

	void move(glm::vec3 m)
	{
		min += m;
		max += m;
	}

	double getWidthX() const
	{
		return max.x - min.x;
	}
	double getWidthZ() const
	{
		return max.z - min.z;
	}
	double getHeight() const
	{
		return max.y - min.y;
	}

	glm::vec3 getPos() const;

	glm::vec3 getDimensions() const;
	bool test(const AABB& aabb) const;
	glm::vec3 getNearestPointTo(glm::vec3 point);
};
