#include <camera.hpp>
#include <cmath>

using namespace std;

Camera::Camera() {

}

Matrix4x4 Camera::perspective(double aspectRatio) const {
	return Matrix4x4::projection(fov, aspectRatio, nearPlane, farPlane);
}

Vector3 Camera::getFront() const {
	Vector3 front { 0.0f, 0.0f, -1.0f };

	Matrix4x4 rotation = Matrix4x4::rotationMatrixXYZ(pitch, yaw, 0.0f);
	front = (Vector3) Matrix4x4::multiply(rotation, front, 1.0f);
	front.normalize();

	return front;
}

Vector3 Camera::getBack() const {
	return getFront().negate();
}

Vector3 Camera::getRight() const {
	return Vector3::cross(getFront(), up).normalize();
}

Vector3 Camera::getLeft() const {
	return getRight().negate();
}

const Vector3& Camera::getPosition() const {
	return position;
}

const Vector3& Camera::getUp() const {
	return up;
}

void Camera::setPosition(const Vector3& v) {
	position = v;
}

void Camera::lookAt(const Vector3& point) {
//	Vector3 dir = (point - position).normalize();
//
//	float yaw = atan2(dir.x, -dir.y);
//	this->yaw = yaw;
//
//	float pitch = atan2(sqrt(dir.x * dir.x + dir.y * dir.y), dir.z);
//	this->pitch = pitch;
}

void Camera::setUp(const Vector3& v) {
	up = v;
}
//yaw -0.519963 pitch 0.0387001
Matrix4x4 Camera::view() const {
	return Matrix4x4::lookAt(position, position + getFront(), up);
}

