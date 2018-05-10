#include "aabb.hpp"

AABB::AABB() {
	this->min = Vector3(0.0f, 0.0f, 0.0f);
	this->max = Vector3(0.0f, 0.0f, 0.0f);
}

AABB::AABB(Vector3 min, Vector3 max) {
	this->min = min;
	this->max = max;
}

Vector3 AABB::getMin() const {
	return min;
}

Vector3 AABB::getMax() const {
	return max;
}
