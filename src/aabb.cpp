#include "aabb.hpp"
#include "math.hpp"
#include "mesh.hpp"

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
	Vector3 minimum(INFINITY, INFINITY, INFINITY);
	Vector3 maximum(-INFINITY, -INFINITY, -INFINITY);

	for (const Triangle* triangle : triangles) {
		for (int v = 0; v < 3; v++) {
			minimum.x = min(minimum.x, (*triangle)[v].position.x);
			minimum.y = min(minimum.y, (*triangle)[v].position.y);
			minimum.z = min(minimum.z, (*triangle)[v].position.z);

			maximum.x = max(maximum.x, (*triangle)[v].position.x);
			maximum.y = max(maximum.y, (*triangle)[v].position.y);
			maximum.z = max(maximum.z, (*triangle)[v].position.z);
		}
	}
	this->minimum = minimum;
	this->maximum = maximum;
}
