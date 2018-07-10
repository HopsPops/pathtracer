#pragma once

#include <math.hpp>

class Camera {
	private:
		double farPlane = 1000.0f;
		float nearPlane = 0.1f;
		Vector3 up { 0.0f, 1.0f, 0.0f };

	public:
		double fov = toRadians(60.0f);
		double pitch = 0.0f;
		float yaw = 0.0f;
		float roll = 0.0f;
		Vector3 position { 0.0f, 0.0f, -1.0f };

		Camera();

		Matrix4x4 perspective(double aspectRatio) const;

		Vector3 getFront() const;
		Vector3 getBack() const;
		Vector3 getRight() const;
		Vector3 getLeft() const;

		const Vector3& getUp() const;
		void setUp(const Vector3& v);

		const Vector3& getPosition() const;
		void setPosition(const Vector3& v);

		void lookAt(const Vector3& point);

		Matrix4x4 view() const;
};
