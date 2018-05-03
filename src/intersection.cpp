#include <glm/detail/type_vec3.hpp>
#include <glm/geometric.hpp>
#include "intersection.hpp"
#include "aabb.hpp"

bool intersectTriangle(const Ray& ray, Vec3 vertex0, Vec3 vertex1, Vec3 vertex2, Vec3& outIntersectionPoint) {
	const float EPSILON = 0.0000001;

	Vec3 rayVector = ray.getDirection();
	Vec3 rayOrigin = ray.getOrigin();

	Vec3 edge1, edge2, h, s, q;

	float a, f, u, v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;

	h = glm::cross(rayVector, edge2);
	a = glm::dot(edge1, h);

	if (a > -EPSILON && a < EPSILON) {
		return false;
	}
	f = 1.0 / a;
	s = rayOrigin - vertex0;
	u = f * glm::dot(s, h);

	if (u < 0.0 || u > 1.0) {
		return false;
	}

	q = glm::cross(s, edge1);
	v = f * glm::dot(rayVector, q);

	if (v < 0.0 || u + v > 1.0) {
		return false;
	}
	float t = f * glm::dot(edge2, q);
	if (t > EPSILON) {
		outIntersectionPoint = rayOrigin + rayVector * t;
		return true;
	} else {
		return false;
	}
}

bool intersectAABB(const Ray& ray, Vec3 min, Vec3 max) {
	float txmin = (min.x - ray.getOrigin().x) / ray.getDirection().x;
	float txmax = (max.x - ray.getOrigin().x) / ray.getDirection().x;

	if (txmin > txmax) {
		float temp = txmin;
		txmin = txmax;
		txmax = temp;
	}

	float tymin = (min.y - ray.getOrigin().y) / ray.getDirection().y;
	float tymax = (max.y - ray.getOrigin().y) / ray.getDirection().y;

	if (tymin > tymax) {
		float temp = tymin;
		tymin = tymax;
		tymax = temp;
	}

	if ((txmin > tymax) || (tymin > txmax))
	return false;

	if (tymin > txmin) {
		txmin = tymin;
	}

	if (tymax < txmax) {
		txmax = tymax;
	}

	float tzmin = (min.z - ray.getOrigin().z) / ray.getDirection().z;
	float tzmax = (max.z - ray.getOrigin().z) / ray.getDirection().z;

	if (tzmin > tzmax) {
		float temp = tzmin;
		tzmin = tzmax;
		tzmax = temp;
	}

	if ((txmin > tzmax) || (tzmin > txmax)) {
		return false;
	}

	if (tzmin > txmin) {
		txmin = tzmin;
	}

	if (tzmax < txmax) {
		txmax = tzmax;
	}

	return true;
}

bool intersectAABB(const Ray& ray, const AABB& aabb) {
	return intersectAABB(ray, aabb.getMin(), aabb.getMax());
}

