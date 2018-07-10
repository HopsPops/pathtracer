#include <math.h>
#include <math.hpp>
#include <cmath>
#include <random>
#include <stdexcept>

float toRadians(float degrees) {
	return degrees * M_PI / 180.0f;
}

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

Matrix4x4 Matrix4x4::rotationMatrixXYZ(float x, float y, float z) {
	float cx = cos(x);
	float cy = cos(y);
	float cz = cos(z);

	float sx = sin(x);
	float sy = sin(y);
	float sz = sin(z);

	Matrix4x4 result = zeroMatrix();
	result.data[0] = cy * cz;
	result.data[1] = -cz * sx * sy - cx * sz;
	result.data[2] = -cx * cz * sy + sx * sz;
	result.data[4] = cy * sz;
	result.data[5] = cx * cz - sx * sy * sz;
	result.data[6] = -cz * sx - cx * sy * sz;
	result.data[8] = sy;
	result.data[9] = cy * sx;
	result.data[10] = cx * cy;

	return result;
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
	result.data[1] = 0.0f;
	result.data[2] = (right + left) / (right - left);
	result.data[3] = 0.0f;

	result.data[4] = 0.0f;
	result.data[5] = 2.0f * near / (top - bottom);
	result.data[6] = (top + bottom) / (top - bottom);
	result.data[7] = 0.0f;

	result.data[8] = 0.0f;
	result.data[9] = 0.0f;
	result.data[10] = -(far + near) / (far - near);
	result.data[11] = -(2.0f * far * near) / (far - near);

	result.data[12] = 0.0f;
	result.data[13] = 0.0f;
	result.data[14] = -1.0f;
	result.data[15] = 0.0f;
	return result;
}

Matrix4x4 Matrix4x4::projection(float fovy, float aspectRatio, float near, float far) {
	float height = near * tan(fovy / 2.0f);
	float width = height * aspectRatio;
	return projection(-width, width, -height, height, near, far);
}

std::array<float, 16> Matrix4x4::getData() {
	return data;
}

Matrix4x4 Matrix4x4::lookAt(const Vector3& eye, const Vector3& center, const Vector3& upDir) {
	Vector3 dir = eye - center;
	dir.normalize();

	Vector3 side = Vector3::cross(upDir, dir);
	side.normalize();

	Vector3 up = Vector3::cross(dir, side);
	up.normalize();

	Matrix4x4 result = identityMatrix();
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

float Vector3::length() const {
	return sqrt(x * x + y * y + z * z);
}

float Vector3::dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 Vector3::cross(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

Vector3 Vector3::operator -(const Vector3& v) const {
	return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator +(const Vector3& v) const {
	return Vector3(x + v.x, y + v.y, z + v.z);
}

const std::array<float, 16> Matrix4x4::getData() const {
	return data;
}

Matrix4x4 Matrix4x4::transpose() {
	Matrix4x4 result = Matrix4x4::zeroMatrix();
	result.data[0] = data[0];
	result.data[1] = data[4];
	result.data[2] = data[8];
	result.data[3] = data[12];
	result.data[4] = data[1];
	result.data[5] = data[5];
	result.data[6] = data[9];
	result.data[7] = data[13];
	result.data[8] = data[2];
	result.data[9] = data[6];
	result.data[10] = data[10];
	result.data[11] = data[14];
	result.data[12] = data[3];
	result.data[13] = data[7];
	result.data[14] = data[11];
	result.data[15] = data[15];
	return result;
}

Matrix4x4::Matrix4x4(std::array<float, 16> data) {
	this->data = data;
}

Matrix4x4::Matrix4x4() {
	this->data = {0.0f};
}

Vector4 Matrix4x4::operator *(const Vector4& v) {
	return multiply(*this, v);
}

//const Vector3 Vector3::operator =(const Vector3& v) {
//	x = v.x;
//	y = v.y;
//	z = v.z;
//	return *this;
//}

const Vector4 Vector4::operator =(const Vector4& v) {
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

Vector4::operator Vector3() const {
	return Vector3(x, y, z);
}

float Vector3::distance(const Vector3& v1, const Vector3& v2) {
	Vector3 sub = v2 - v1;
	return sub.length();
}

Vector3& Vector3::negate() {
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

Vector3 operator *(float factor, const Vector3& vector) {
	return Vector3(vector.x * factor, vector.y * factor, vector.z * factor);
}

Vector3 operator *(const Vector3& vector, float factor) {
	return Vector3(vector.x * factor, vector.y * factor, vector.z * factor);
}

bool Vector3::isZero() const {
	return x == 0.0f && y == 0.0f && z == 0.0f;
}

float Vector3::operator [](int i) const {
	if (i == 0) {
		return x;
	} else if (i == 1) {
		return y;
	} else if (i == 2) {
		return z;
	} else {
		return operator [](i % 3);
	}
}

float* Vector3::operator [](int i) {
	if (i == 0) {
		return &x;
	} else if (i == 1) {
		return &y;
	} else if (i == 2) {
		return &z;
	} else {
		throw std::invalid_argument("i > 2");
	}
}

float Vector3::cosineAngle(const Vector3& v1, const Vector3& v2) {
	return Vector3::dot(v1, v2) / (v1.length() * v2.length());
}

const Vector3& Vector3::operator +=(const Vector3& v) {
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	return *this;
}

Vector3 Vector3::operator /(float factor) const {
	return Vector3 { x / factor, y / factor, z / factor };
}

Vector3 Vector3::reflection(const Vector3& dir, const Vector3& normal) {
	return (dir - (2 * Vector3::dot(dir, normal) * normal)).normalize();
}

Vector3& Vector3::clamp(float ceiling) {
	this->x = x > ceiling ? ceiling : x;
	this->y = y > ceiling ? ceiling : y;
	this->z = z > ceiling ? ceiling : z;
	return *this;
}

const Vector3& Vector3::operator -=(const Vector3& v) {
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	return *this;
}

std::ostream& operator<<(std::ostream &stream, const Vector3& vec) {
	stream << "{ " << vec.x << ", " << vec.y << ", " << vec.z << " }";
	return stream;
}

Vector3 Vector3::copy() const {
	return Vector3(x, y, z);
}

Vector3& Vector3::scale(float factor) {
	x *= factor;
	y *= factor;
	z *= factor;
	return *this;
}

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector4::Vector4() {

}

Vector4::Vector4(float x, float y, float z, float w) :
		x(x), y(y), z(z), w(w) {
}

Vector4::Vector4(Vector3 v, float w) :
		x(v.x), y(v.y), z(v.z), w(w) {
}

float clamp(float value, float ceiling) {
	if (value > ceiling) {
		return ceiling;
	}
	return value;
}

Vector3 operator *(const Vector3& v1, const Vector3& v2) {
	return Vector3 { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

//std::random_device rd;

//http://corysimon.github.io/articles/uniformdistn-on-sphere/
Vector3 randomVectorOnSphere() {
	static std::mt19937 gen(0);
	static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

	float theta = 2.0f * M_PI * distribution(gen);
	float phi = acos(1 - 2 * distribution(gen));
	float x = sin(phi) * cos(theta);
	float y = sin(phi) * sin(theta);
	float z = cos(phi);

	Vector3 dir { x, y, z };
	return dir.normalize();
}

Vector3 randomVectorOnHemisphere(const Vector3& normal) {
	Vector3 dir = randomVectorOnSphere();
	if (Vector3::dot(normal, dir) < 0.0f) {
		dir.negate();
	}
	return dir.normalize();
}

Vector2::Vector2() {
	this->x = 0.0f;
	this->y = 0.0f;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

Vector3 Vector3::min(const Vector3& v1, const Vector3& v2) {
	return Vector3 { fmin(v1.x, v2.x), fmin(v1.y, v2.y), fmin(v1.z, v2.z) };
}

Vector3 Vector3::max(const Vector3& v1, const Vector3& v2) {
	return Vector3 { fmax(v1.x, v2.x), fmax(v1.y, v2.y), fmax(v1.z, v2.z) };
}
