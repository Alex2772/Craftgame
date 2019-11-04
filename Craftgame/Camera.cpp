#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "GameEngine.h"

Camera::Camera() {

}

glm::mat4 Camera::createViewMatrix() {
	glm::mat4 view;
	view = glm::rotate(view, glm::radians(-pitch), glm::vec3(1, 0, 0));
	view = glm::rotate(view, glm::radians(-yaw), glm::vec3(0, 1, 0));
	view = glm::translate(view, glm::vec3(-pos.x, -pos.y, -pos.z));
	return view;
}
void Camera::reset() {
	pos = glm::vec3(0, 0, 0);
	yaw = 0;
	pitch = 0;
}

void Camera::upload()
{
#ifndef SERVER
	if (CGE::instance->staticShader) {
		size_t view = CGE::instance->staticShader->getUniform("view");
		size_t viewPos = CGE::instance->staticShader->getUniform("viewPos");
		CGE::instance->staticShader->loadMatrix(view, createViewMatrix());
		CGE::instance->staticShader->loadVector(viewPos, pos);
	}
#endif
}
