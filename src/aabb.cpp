#include <aabb.hpp>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

AABB::AABB() {
	this->minimum = Vector3(0.0f, 0.0f, 0.0f);
	this->maximum = Vector3(0.0f, 0.0f, 0.0f);
}

AABB::AABB(const Vector3& min, const Vector3& max) {
	this->minimum = min;
	this->maximum = max;
}

const Vector3& AABB::getMin() const {
	return minimum;
}

const Vector3& AABB::getMax() const {
	return maximum;
}

AABB::AABB(const Triangles& triangles) {
	Vector3 minimum(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity());
	Vector3 maximum(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity());

	for (const Triangle* triangle : triangles) {
		for (int v = 0; v < 3; v++) {
			minimum.x = fmin(minimum.x, (*triangle)[v].position.x);
			minimum.y = fmin(minimum.y, (*triangle)[v].position.y);
			minimum.z = fmin(minimum.z, (*triangle)[v].position.z);

			maximum.x = fmax(maximum.x, (*triangle)[v].position.x);
			maximum.y = fmax(maximum.y, (*triangle)[v].position.y);
			maximum.z = fmax(maximum.z, (*triangle)[v].position.z);
		}
	}
	this->minimum = minimum;
	this->maximum = maximum;
}

AABB::AABB(const vector<Vector3>& list) {
	Vector3 minimum(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity());
	Vector3 maximum(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity());

	for (const Vector3& v : list) {
		minimum.x = fmin(minimum.x, v.x);
		minimum.y = fmin(minimum.y, v.y);
		minimum.z = fmin(minimum.z, v.z);

		maximum.x = fmax(maximum.x, v.x);
		maximum.y = fmax(maximum.y, v.y);
		maximum.z = fmax(maximum.z, v.z);
	}

	this->minimum = minimum;
	this->maximum = maximum;
}

AABB::AABB(const AABB& aabb1, const AABB& aabb2) {
	this->minimum = Vector3::min(aabb1.getMin(), aabb2.getMin());
	this->maximum = Vector3::max(aabb1.getMax(), aabb2.getMax());
}

Vector3 AABB::center() const {
	return Vector3 { (minimum.x + maximum.x) / 2.0f, (minimum.y + maximum.y) / 2.0f, (minimum.z + maximum.z) / 2.0f };
}
