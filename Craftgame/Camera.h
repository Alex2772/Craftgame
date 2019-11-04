#pragma once

#include <glm/glm.hpp>

class Camera {
public:
	Camera();
	glm::vec3 pos = glm::vec3(0, 0, 0);
	float yaw = 0;
	float pitch = 0;
	glm::mat4 createViewMatrix();
	void reset();
	void upload();
};