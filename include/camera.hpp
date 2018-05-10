#pragma once
#include "math.hpp"

class Camera {
	private:
		double fov = toRadians(60.0f);
		double pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
		double farPlane = 1000.0f, nearPlane = 0.1f;
		Vector3 position = Vector3(0.0f, 0.0f, -1.0f);
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

	public:
		Camera() {

		}

		Matrix4x4 perspective(double aspectRatio) const {
			return Matrix4x4::projection(fov, aspectRatio, nearPlane, farPlane);
		}

		Vector3 getFront() const {
			Vector3 front = Vector3(0.0f, 0.0f, -1.0f);

			Matrix4x4 rotation = Matrix4x4::rotationMatrixXYZ(pitch, yaw, 0);
			front = (Vector3) Matrix4x4::multiply(rotation, front, 0.0f);
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
		void lookAt(Vector3 v) {

		}

		void setUp(Vector3 v) {
			up = v;
		}

		Matrix4x4 view() const {
			return Matrix4x4::lookAt(position, position + getFront(), up);
		}

};
