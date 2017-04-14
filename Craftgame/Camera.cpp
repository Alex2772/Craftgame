#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {

}

glm::mat4 Camera::createViewMatrix() {
	glm::mat4 view;
	view = glm::rotate(view, glm::radians(pitch), glm::vec3(1, 0, 0));
	view = glm::rotate(view, glm::radians(yaw), glm::vec3(0, 1, 0));
	view = glm::translate(view, glm::vec3(-pos.x, -pos.y, -pos.z));
	return view;
}