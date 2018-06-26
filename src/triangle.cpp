#include <triangle.hpp>
#include <math.hpp>

Vector3 Triangle::middle() {
	float x = (v1.position.x + v2.position.x + v3.position.x) / 3.0;
	float y = (v1.position.y + v2.position.y + v3.position.y) / 3.0;
	float z = (v1.position.z + v2.position.z + v3.position.z) / 3.0;

	Vector3 vec { x, y, z };
	return vec;
}

Vertex Triangle::operator [](int i) const {
	if (i == 0) {
		return v1;
	} else if (i == 1) {
		return v2;
	} else if (i == 2) {
		return v3;
	} else {
		throw std::invalid_argument("i > 2");
	}
}

double trianglesMedian(const Triangles& triangles, int axis) {
	double median = 0.0;

	for (const Triangle* triangle : triangles) {
		median += triangle->v1.position[axis] / (double) triangles.size() / 3.0;
		median += triangle->v2.position[axis] / (double) triangles.size() / 3.0;
		median += triangle->v3.position[axis] / (double) triangles.size() / 3.0;
	}

	return median;
}

