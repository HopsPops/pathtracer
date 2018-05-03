#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "types.hpp"

class Camera {
	private:
		double fov = glm::radians(60.0f);
		double pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
		double farPlane = 1000.0f, nearPlane = 0.1f;
		Vec3 position = Vec3(0.0f, 0.0f, -1.0f);
		Vec3 up = Vec3(0.0f, 1.0f, 0.0f);

	public:
		Camera() {

		}

		Mat4 perspective(double aspectRatio) const {
			return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
		}

		Vec3 getFront() const {
			Vec3 front = Vec3(0.0f, 0.0f, -1.0f);

			Mat4 rotation = glm::eulerAngleXY(pitch, yaw);
			front = rotation * Vec4(front, 0.0f);
			glm::normalize(front);

			return front;
		}

		Vec3 getPosition() const {
			return position;
		}

		void setPosition(Vec3 v) {
			position = v;
		}

		//TODO
		void lookAt(Vec3 v) {

		}

		void setUp(Vec3 v) {
			up = v;
		}

		glm::mat4x4 view() const {
			return glm::lookAt(position, position + getFront(), up);
		}

};
