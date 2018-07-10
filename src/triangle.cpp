#include <triangle.hpp>
#include <cmath>
#include <random>

using namespace std;

Vector3 Triangle::middle() const {
	float x = (v1.position.x + v2.position.x + v3.position.x) / 3.0;
	float y = (v1.position.y + v2.position.y + v3.position.y) / 3.0;
	float z = (v1.position.z + v2.position.z + v3.position.z) / 3.0;

	Vector3 vec { x, y, z };
	return vec;
}

const Vertex& Triangle::operator [](int i) const {
	if (i == 0) {
		return v1;
	} else if (i == 1) {
		return v2;
	} else if (i == 2) {
		return v3;
	} else {
		return operator [](i % 3);
	}
}

Triangle::Triangle(long id, Vertex v1, Vertex v2, Vertex v3, Vector3 t1, Vector3 t2, const Material* mat) {
	this->id = id;
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
	this->tangent1 = t1;
	this->tangent2 = t2;
	this->material = mat;
}

float trianglesMedian(const Triangles& triangles, int axis) {
	double median = 0.0;

	for (const Triangle* triangle : triangles) {
		median += triangle->v1.position[axis] / (double) triangles.size() / 3.0;
		median += triangle->v2.position[axis] / (double) triangles.size() / 3.0;
		median += triangle->v3.position[axis] / (double) triangles.size() / 3.0;
	}

	return median;
}

bool Triangle::isCounterclockwise() const {
	Vector3 v1 = this->v1.position;
	Vector3 v2 = this->v2.position;
	Vector3 v3 = this->v3.position;

	double det = 1.0f / 2.0f * Vector3::cross(v2 - v1, v3 - v1).length();
	return det >= 0.0f;
}

float Triangle::area() const {
	float det = Vector3::cross(tangent1, tangent2).length() / 2.0f;
	return det;
}

Vector3 Triangle::normal() const {
	const Vector3& u = tangent1;
	const Vector3& v = tangent2;

	Vector3 n { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };
	n.normalize();
	return n;
}

Vector3 Triangle::randomPointOnTriangle() const {
	static mt19937 triangleGen(0);
	static uniform_real_distribution<float> triangleDistr(0.0f, 1.0f);

	float l1 = triangleDistr(triangleGen);
	float l2 = triangleDistr(triangleGen);
	float l3 = triangleDistr(triangleGen);

	float sum = l1 + l2 + l3;
	l1 /= sum;
	l2 /= sum;
	l3 /= sum;

	return l1 * v1.position + l2 * v2.position + l3 * v3.position;
}

bool Triangle::isLight() const {
	return !material->emmisive.isZero();
}

float Triangle::minAxis(int axis) const {
	return fmin(v1.position[axis], fmin(v2.position[axis], v3.position[axis]));
}

float Triangle::maxAxis(int axis) const {
	return fmax(v1.position[axis], fmax(v2.position[axis], v3.position[axis]));
}

Triangles findLights(const Triangles& triangles) {
	Triangles lights { };
	for (const Triangle* triangle : triangles) {
		if (triangle->isLight()) {
			lights.push_back(triangle);
		}
	}
	return lights;
}
