#pragma once
#include "math.hpp"

class Camera {
	private:
		double farPlane = 1000.0f, nearPlane = 0.1f;
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

	public:
		double fov = toRadians(60.0f);
		double pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
		Vector3 position = Vector3(0.0f, 0.0f, -1.0f);

		Camera() {

		}

		Matrix4x4 perspective(double aspectRatio) const {
			return Matrix4x4::projection(fov, aspectRatio, nearPlane, farPlane);
		}

		Vector3 getFront() const {
			Vector3 front = Vector3(0.0f, 0.0f, -1.0f);

			Matrix4x4 rotation = Matrix4x4::rotationMatrixXYZ(pitch, yaw, 0.0f);
			front = (Vector3) Matrix4x4::multiply(rotation, front, 1.0f);
			front.normalize();

			return front;
		}

		Vector3 getPosition() const {
			return position;
		}

		void setPosition(Vector3 v) {
			position = v;
		}

		//TODO
		void lookAt(const Vector3& point) {
			Vector3 dir = (point - position).normalize();

			float yaw = 0.0f;
//			if (dir.x == 0.0f && dir.y == 0.0f) {
//				yaw = 0.0f;
//			} else {
				yaw = atan2(dir.x, -dir.y);
//			}
			this->yaw = yaw;

			float pitch = 0.0f;
//			if (dir.x == 0.0f && dir.y == 0.0f && dir.z == 0.0f) {
//				pitch = 0.0f;
//			} else {
				pitch = atan2(sqrt(dir.x * dir.x + dir.y * dir.y), dir.z);
//			}
			this->pitch = pitch;

		}

		void setUp(Vector3 v) {
			up = v;
		}

		Matrix4x4 view() const {
			return Matrix4x4::lookAt(position, position + getFront(), up);
		}

};
