#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "types.hpp"

class camera {
	private:
		double fov = glm::radians(60.0f);
		double pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
		double farPlane = 1000.0f, nearPlane = 0.1f;
		vec3 position = vec3(0.0f, 0.0f, -1.0f);
		vec3 up = vec3(0.0f, 1.0f, 0.0f);

	public:
		camera() {

		}

		glm::mat4x4 perspective(double aspectRatio) const {
			return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
		}

		vec3 getFront() const {
			vec3 front = vec3(0.0f, 0.0f, -1.0f);

			mat4 rotation = glm::eulerAngleXY(pitch, yaw);
			front = rotation * glm::vec4(front, 0.0f);
			glm::normalize(front);

			return front;
		}

		vec3 getPosition() const {
			return position;
		}

		void setPosition(vec3 v) {
			position = v;
		}

		//TODO
		void lookAt(vec3 v) {

		}

		void setUp(vec3 v) {
			up = v;
		}

		glm::mat4x4 view() const {
			return glm::lookAt(position, position + getFront(), up);
		}

};
