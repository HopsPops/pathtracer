#include "math.hpp"
#include "cmath"

Matrix4x4 Matrix4x4::zeroMatrix() {
	return Matrix4x4();
}

Matrix4x4 Matrix4x4::identityMatrix() {
	Matrix4x4 result = zeroMatrix();
	result.data[0] = 1.0f;
	result.data[5] = 1.0f;
	result.data[10] = 1.0f;
	result.data[15] = 1.0f;
	return result;
}

Matrix4x4 Matrix4x4::translationMatrix(Vector3 translation) {
	Matrix4x4 result = identityMatrix();
	result.data[3] = translation.x;
	result.data[7] = translation.y;
	result.data[11] = translation.z;
	return result;
}

Matrix4x4 Matrix4x4::rotationMatrix() {
}

Matrix4x4 Matrix4x4::multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = zeroMatrix();

	result.data[0] = m1.data[0] * m2.data[0] + m1.data[1] * m2.data[4] + m1.data[2] * m2.data[8] + m1.data[3] * m2.data[12];
	result.data[1] = m1.data[0] * m2.data[1] + m1.data[1] * m2.data[5] + m1.data[2] * m2.data[9] + m1.data[3] * m2.data[13];
	result.data[2] = m1.data[0] * m2.data[2] + m1.data[1] * m2.data[6] + m1.data[2] * m2.data[10] + m1.data[3] * m2.data[14];
	result.data[3] = m1.data[0] * m2.data[3] + m1.data[1] * m2.data[7] + m1.data[2] * m2.data[11] + m1.data[3] * m2.data[15];
	result.data[4] = m1.data[4] * m2.data[0] + m1.data[5] * m2.data[4] + m1.data[6] * m2.data[8] + m1.data[7] * m2.data[12];
	result.data[5] = m1.data[4] * m2.data[1] + m1.data[5] * m2.data[5] + m1.data[6] * m2.data[9] + m1.data[7] * m2.data[13];
	result.data[6] = m1.data[4] * m2.data[2] + m1.data[5] * m2.data[6] + m1.data[6] * m2.data[10] + m1.data[7] * m2.data[14];
	result.data[7] = m1.data[4] * m2.data[3] + m1.data[5] * m2.data[7] + m1.data[6] * m2.data[11] + m1.data[7] * m2.data[15];
	result.data[8] = m1.data[8] * m2.data[0] + m1.data[9] * m2.data[4] + m1.data[10] * m2.data[8] + m1.data[11] * m2.data[12];
	result.data[9] = m1.data[8] * m2.data[1] + m1.data[9] * m2.data[5] + m1.data[10] * m2.data[9] + m1.data[11] * m2.data[13];
	result.data[10] = m1.data[8] * m2.data[2] + m1.data[9] * m2.data[6] + m1.data[10] * m2.data[10] + m1.data[11] * m2.data[14];
	result.data[11] = m1.data[8] * m2.data[3] + m1.data[9] * m2.data[7] + m1.data[10] * m2.data[11] + m1.data[11] * m2.data[15];
	result.data[12] = m1.data[12] * m2.data[0] + m1.data[13] * m2.data[4] + m1.data[14] * m2.data[8] + m1.data[15] * m2.data[12];
	result.data[13] = m1.data[12] * m2.data[1] + m1.data[13] * m2.data[5] + m1.data[14] * m2.data[9] + m1.data[15] * m2.data[13];
	result.data[14] = m1.data[12] * m2.data[2] + m1.data[13] * m2.data[6] + m1.data[14] * m2.data[10] + m1.data[15] * m2.data[14];
	result.data[15] = m1.data[12] * m2.data[3] + m1.data[13] * m2.data[7] + m1.data[14] * m2.data[11] + m1.data[15] * m2.data[15];

	return result;
}

Vector4 Matrix4x4::multiply(const Matrix4x4& m, const Vector4& v) {
	return multiply(m, v.x, v.y, v.z, v.w);
}

Vector4 Matrix4x4::multiply(const Matrix4x4& m, const Vector3& v, float w) {
	return multiply(m, v.x, v.y, v.z, w);
}

Vector4 Matrix4x4::multiply(const Matrix4x4& m, float x, float y, float z, float w) {
	return Vector4(x * m.data[0] + y * m.data[1] + z * m.data[2] + w * m.data[3], x * m.data[4] + y * m.data[5] + z * m.data[6] + w * m.data[7],
			x * m.data[8] + y * m.data[9] + z * m.data[10] + w * m.data[11], x * m.data[12] + y * m.data[13] + z * m.data[14] + w * m.data[15]);
}

float Matrix4x4::det() {
	float determinant =
			data[3] * data[6] * data[9] * data[12] -
					data[2] * data[7] * data[9] * data[12] -
					data[3] * data[5] * data[10] * data[12] +
					data[1] * data[7] * data[10] * data[12] +
					data[2] * data[5] * data[11] * data[12] -
					data[1] * data[6] * data[11] * data[12] -
					data[3] * data[6] * data[8] * data[13] +
					data[2] * data[7] * data[8] * data[13] +
					data[3] * data[4] * data[10] * data[13] -
					data[0] * data[7] * data[10] * data[13] -
					data[2] * data[4] * data[11] * data[13] +
					data[0] * data[6] * data[11] * data[13] +
					data[3] * data[5] * data[8] * data[14] -
					data[1] * data[7] * data[8] * data[14] -
					data[3] * data[4] * data[9] * data[14] +
					data[0] * data[7] * data[9] * data[14] +
					data[1] * data[4] * data[11] * data[14] -
					data[0] * data[5] * data[11] * data[14] -
					data[2] * data[5] * data[8] * data[15] +
					data[1] * data[6] * data[8] * data[15] +
					data[2] * data[4] * data[9] * data[15] -
					data[0] * data[6] * data[9] * data[15] -
					data[1] * data[4] * data[10] * data[15] +
					data[0] * data[5] * data[10] * data[15];
	return determinant;
}

Matrix4x4 Matrix4x4::inverse() {
	Matrix4x4 result = Matrix4x4::zeroMatrix();
	float det = this->det();
	result.data[0] = (-(data[7] * data[10] * data[13]) + data[6] * data[11] * data[13] + data[7] * data[9] * data[14] - data[5] * data[11] * data[14] - data[6] * data[9] * data[15] + data[5] * data[10] * data[15]) / det;
	result.data[1] = (data[3] * data[10] * data[13] - data[2] * data[11] * data[13] - data[3] * data[9] * data[14] + data[1] * data[11] * data[14] + data[2] * data[9] * data[15] - data[1] * data[10] * data[15]) / det;
	result.data[2] = (-(data[3] * data[6] * data[13]) + data[2] * data[7] * data[13] + data[3] * data[5] * data[14] - data[1] * data[7] * data[14] - data[2] * data[5] * data[15] + data[1] * data[6] * data[15]) / det;
	result.data[3] = (data[3] * data[6] * data[9] - data[2] * data[7] * data[9] - data[3] * data[5] * data[10] + data[1] * data[7] * data[10] + data[2] * data[5] * data[11] - data[1] * data[6] * data[11]) / det;
	result.data[4] = (data[7] * data[10] * data[12] - data[6] * data[11] * data[12] - data[7] * data[8] * data[14] + data[4] * data[11] * data[14] + data[6] * data[8] * data[15] - data[4] * data[10] * data[15]) / det;
	result.data[5] = (-(data[3] * data[10] * data[12]) + data[2] * data[11] * data[12] + data[3] * data[8] * data[14] - data[0] * data[11] * data[14] - data[2] * data[8] * data[15] + data[0] * data[10] * data[15]) / det;
	result.data[6] = (data[3] * data[6] * data[12] - data[2] * data[7] * data[12] - data[3] * data[4] * data[14] + data[0] * data[7] * data[14] + data[2] * data[4] * data[15] - data[0] * data[6] * data[15]) / det;
	result.data[7] = (-(data[3] * data[6] * data[8]) + data[2] * data[7] * data[8] + data[3] * data[4] * data[10] - data[0] * data[7] * data[10] - data[2] * data[4] * data[11] + data[0] * data[6] * data[11]) / det;
	result.data[8] = (-(data[7] * data[9] * data[12]) + data[5] * data[11] * data[12] + data[7] * data[8] * data[13] - data[4] * data[11] * data[13] - data[5] * data[8] * data[15] + data[4] * data[9] * data[15]) / det;
	result.data[9] = (data[3] * data[9] * data[12] - data[1] * data[11] * data[12] - data[3] * data[8] * data[13] + data[0] * data[11] * data[13] + data[1] * data[8] * data[15] - data[0] * data[9] * data[15]) / det;
	result.data[10] = (-(data[3] * data[5] * data[12]) + data[1] * data[7] * data[12] + data[3] * data[4] * data[13] - data[0] * data[7] * data[13] - data[1] * data[4] * data[15] + data[0] * data[5] * data[15]) / det;
	result.data[11] = (data[3] * data[5] * data[8] - data[1] * data[7] * data[8] - data[3] * data[4] * data[9] + data[0] * data[7] * data[9] + data[1] * data[4] * data[11] - data[0] * data[5] * data[11]) / det;
	result.data[12] = (data[6] * data[9] * data[12] - data[5] * data[10] * data[12] - data[6] * data[8] * data[13] + data[4] * data[10] * data[13] + data[5] * data[8] * data[14] - data[4] * data[9] * data[14]) / det;
	result.data[13] = (-(data[2] * data[9] * data[12]) + data[1] * data[10] * data[12] + data[2] * data[8] * data[13] - data[0] * data[10] * data[13] - data[1] * data[8] * data[14] + data[0] * data[9] * data[14]) / det;
	result.data[14] = (data[2] * data[5] * data[12] - data[1] * data[6] * data[12] - data[2] * data[4] * data[13] + data[0] * data[6] * data[13] + data[1] * data[4] * data[14] - data[0] * data[5] * data[14]) / det;
	result.data[15] = (-(data[2] * data[5] * data[8]) + data[1] * data[6] * data[8] + data[2] * data[4] * data[9] - data[0] * data[6] * data[9] - data[1] * data[4] * data[10] + data[0] * data[5] * data[10]) / det;
	return result;
}

void Matrix4x4::scale(float factor) {
	for (int i = 0; i < 16; i++) {
		data[i] *= factor;
	}
}

Matrix4x4 Matrix4x4::projection(float left, float right, float bottom, float top, float near, float far) {
	Matrix4x4 result = Matrix4x4::zeroMatrix();
	result.data[0] = 2.0f * near / (right - left);
	result.data[0] = 0.0f;
	result.data[0] = (right + left) / (right - left);
	result.data[0] = 0.0f;

	result.data[0] = 0.0f;
	result.data[0] = 2.0f * near / (top - bottom);
	result.data[0] = (top + bottom) / (top - bottom);
	result.data[0] = 0.0f;

	result.data[0] = 0.0f;
	result.data[0] = 0.0f;
	result.data[0] = -(far + near) / (far - near);
	result.data[0] = -(2.0f * far * near) / (far - near);

	result.data[0] = 0.0f;
	result.data[0] = 0.0f;
	result.data[0] = -1.0f;
	result.data[0] = 0.0f;
	return result;
}

Matrix4x4 Matrix4x4::projection(float fovy, float aspectRatio, float near, float far) {
//	float tan = ;
	float height = near * tan(fovy / 2.0);
	float width = height * aspectRatio;
	return projection(-width, width, -height, height, near, far);
}

std::array<float, 16> Matrix4x4::getData() {
	return data;
}

Matrix4x4 Matrix4x4::lookAt(Vector3 eye, Vector3 center, Vector3 upDir) {
	Vector3 dir = eye - center;
	dir.normalize();

	Vector3 side = Vector3::cross(upDir, dir);
	side.normalize();

	Vector3 up = Vector3::cross(dir, side);
	up.normalize();

	Matrix4x4 result = zeroMatrix();
	result.data[0] = side.x;
	result.data[1] = side.y;
	result.data[2] = side.z;

	result.data[4] = up.x;
	result.data[5] = up.y;
	result.data[6] = up.z;

	result.data[8] = dir.x;
	result.data[9] = dir.y;
	result.data[10] = dir.z;

	result.data[3] = -Vector3::dot(eye, side);
	result.data[7] = -Vector3::dot(eye, up);
	result.data[11] = -Vector3::dot(eye, dir);
	return result;
}

Matrix4x4 Matrix4x4::operator *(const Matrix4x4& m) {
	return multiply(*this, m);
}

Vector4 Vector4::copy() {
	return Vector4(x, y, z, w);
}

Vector3& Vector3::normalize() {
	float l = length();
	x /= l;
	y /= l;
	z /= l;
	return *this;
}

float Vector3::length() {
	return sqrt(x * x + y * y + z * z);
}

float Vector3::dot(Vector3 v1, Vector3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 Vector3::cross(Vector3 v1, Vector3 v2) {
	return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

Vector3 Vector3::operator -(Vector3 v) {
	return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator +(Vector3 v) {
	return Vector3(x + v.x, y + v.y, z + v.z);
}
