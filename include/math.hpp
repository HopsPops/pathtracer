#pragma once

#include <array>
#include <iostream>

const double M_PI = 3.14159265358979323846;

float toRadians(float degrees);

float clamp(float, float = 1.0f);
class Vector3;

Vector3 randomVectorOnSphere();
Vector3 randomVectorOnHemisphere(const Vector3&);

class Vector2 {
	public:
		float x = 0.0f;
		float y = 0.0f;
		Vector2();
		Vector2(float, float);
};

class Vector3 {
	public:
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		Vector3& normalize(); //return this
		Vector3 normalize() const;
		Vector3& negate();
		Vector3& scale(float);
		float length() const;


		Vector3() = default;
		Vector3(float x, float y, float z);

		static float cosineAngle(const Vector3&, const Vector3&);

		static float dot(const Vector3&, const Vector3&);
		static Vector3 cross(const Vector3&, const Vector3&);
		static float distance(const Vector3&, const Vector3&);
		static Vector3 min(const Vector3&, const Vector3&);
		static Vector3 max(const Vector3&, const Vector3&);
		Vector3 operator-(const Vector3&) const;
		Vector3 operator/(float) const;
		Vector3 operator+(const Vector3&) const;
		float operator [](int i) const;
		float* operator [](int i);
		Vector3 copy() const;
		Vector3& clamp(float); //return this
		Vector3 reflection(const Vector3& dir, const Vector3& normal);

//		const Vector3& operator=(const Vector3&) = default;
		const Vector3& operator+=(const Vector3&);
		const Vector3& operator-=(const Vector3&);
		bool isZero() const;
		friend std::ostream& operator<<(std::ostream&, const Vector3&);
};

struct Material {
		Vector3 diffuse {};
		Vector3 emmisive {};
};


Vector3 operator*(float, const Vector3&);
Vector3 operator*(const Vector3&, float);
Vector3 operator*(const Vector3&, const Vector3&);

class Vector4 {
	public:
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;

		Vector4();
		Vector4(float x, float y, float z, float w);
		Vector4(Vector3 v, float w);

		const Vector4 operator=(const Vector4&);
		operator Vector3() const;

		Vector4 copy();
};

class Matrix4x4 {
	private:
		std::array<float, 16> data { 0.0f };
		Matrix4x4();

	public:
		Matrix4x4(std::array<float, 16> data);
		static Matrix4x4 zeroMatrix();
		static Matrix4x4 identityMatrix();
		static Matrix4x4 translationMatrix(Vector3);
		static Matrix4x4 rotationMatrixXYZ(float x, float y, float z);
		static Matrix4x4 projection(float left, float right, float bottom, float top, float near, float far);
		static Matrix4x4 projection(float fovy, float aspectRatio, float near, float far);
		static Matrix4x4 lookAt(const Vector3& eye, const Vector3& center, const Vector3& upDir);

		static Matrix4x4 multiply(const Matrix4x4&, const Matrix4x4&);
		static Vector4 multiply(const Matrix4x4&, const Vector4&);
		static Vector4 multiply(const Matrix4x4&, const Vector3&, float w);
		static Vector4 multiply(const Matrix4x4&, float x, float y, float z, float w);

		float det();
		Matrix4x4 inverse();
		Matrix4x4 transpose();
		void scale(float factor);

		std::array<float, 16> getData();
		const std::array<float, 16> getData() const;
		Matrix4x4 operator*(const Matrix4x4&);
		Vector4 operator*(const Vector4&);

};

