#include "aabb.hpp"
#include "math.hpp"
#include "mesh.hpp"

AABB::AABB() {
	this->minimum = Vector3(0.0f, 0.0f, 0.0f);
	this->maximum = Vector3(0.0f, 0.0f, 0.0f);
}

AABB::AABB(Vector3 min, Vector3 max) {
	this->minimum = min;
	this->maximum = max;
}

Vector3 AABB::getMin() const {
	return minimum;
}

Vector3 AABB::getMax() const {
	return maximum;
}

AABB::AABB(const Triangles& triangles) {
	Vector3 minimum(INFINITY, INFINITY, INFINITY);
	Vector3 maximum(-INFINITY, -INFINITY, -INFINITY);

	for (const Triangle* triangle : triangles) {
		minimum.x = min(minimum.x, triangle->v1.position.x);
		minimum.y = min(minimum.y, triangle->v1.position.y);
		minimum.z = min(minimum.z, triangle->v1.position.z);
		maximum.x = max(maximum.x, triangle->v1.position.x);
		maximum.y = max(maximum.y, triangle->v1.position.y);
		maximum.z = max(maximum.z, triangle->v1.position.z);

		minimum.x = min(minimum.x, triangle->v2.position.x);
		minimum.y = min(minimum.y, triangle->v2.position.y);
		minimum.z = min(minimum.z, triangle->v2.position.z);
		maximum.x = max(maximum.x, triangle->v2.position.x);
		maximum.y = max(maximum.y, triangle->v2.position.y);
		maximum.z = max(maximum.z, triangle->v2.position.z);

		minimum.x = min(minimum.x, triangle->v3.position.x);
		minimum.y = min(minimum.y, triangle->v3.position.y);
		minimum.z = min(minimum.z, triangle->v3.position.z);
		maximum.x = max(maximum.x, triangle->v3.position.x);
		maximum.y = max(maximum.y, triangle->v3.position.y);
		maximum.z = max(maximum.z, triangle->v3.position.z);
	}
	this->minimum = minimum;
	this->maximum = maximum;
}
