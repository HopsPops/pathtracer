#include "aabb.hpp"

AABB::AABB() {
	this->min = Vec3(0.0f, 0.0f, 0.0f);
	this->max = Vec3(0.0f, 0.0f, 0.0f);
}

AABB::AABB(Vec3 min, Vec3 max) {
	this->min = min;
	this->max = max;
}

Vec3 AABB::getMin() const {
	return min;
}

Vec3 AABB::getMax() const {
	return max;
}
