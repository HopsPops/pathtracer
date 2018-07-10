#include <bits/move.h>
#include <math.hpp>
#include <ray.hpp>
#include <triangle.hpp>
#include <vertex.hpp>
#include "intersection.hpp"
#include <limits>

using namespace std;

unique_ptr<Vector3> intersectTriangle(const Ray& ray, const Vector3& vertex0, const Vector3& vertex1, const Vector3& vertex2) {
	const float EPSILON = 0.000001f;

	Vector3 rayVector = ray.getDirection();
	Vector3 rayOrigin = ray.getOrigin();

	Vector3 edge1, edge2, h, s, q;

	float a = 0.0, f = 0.0, u = 0.0, v = 0.0;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;

	h = Vector3::cross(rayVector, edge2);
	a = Vector3::dot(edge1, h);

	if (a > -EPSILON && a < EPSILON) {
		return unique_ptr<Vector3>(nullptr);
	}
	f = 1.0 / a;
	s = rayOrigin - vertex0;
	u = f * Vector3::dot(s, h);

	if (u < 0.0 || u > 1.0) {
		return unique_ptr<Vector3>(nullptr);
	}

	q = Vector3::cross(s, edge1);
	v = f * Vector3::dot(rayVector, q);

	if (v < 0.0 || u + v > 1.0) {
		return unique_ptr<Vector3>(nullptr);
	}
	double t = f * Vector3::dot(edge2, q);
	if (t > EPSILON) {
		Vector3* result = new Vector3;
		*result = rayOrigin + rayVector * t;
		return unique_ptr<Vector3>(result);
	} else {
		return unique_ptr<Vector3>(nullptr);
	}
}

unique_ptr<Vector3> intersectTriangle(const Ray& ray, const Triangle& triangle) {
	const float EPSILON = 0.000001f;

	const Vector3& rayDirection = ray.getDirection();
	const Vector3& rayOrigin = ray.getOrigin();

	Vector3 h, s, q;

	float a = 0.0, f = 0.0, u = 0.0, v = 0.0;
	const Vector3& edge1 = triangle.tangent1;
	const Vector3& edge2 = triangle.tangent2;

	h = Vector3::cross(rayDirection, edge2);
	a = Vector3::dot(edge1, h);

	if (a > -EPSILON && a < EPSILON) {
		return unique_ptr<Vector3>(nullptr);
	}
	f = 1.0 / a;
	s = rayOrigin - triangle.v1.position;
	u = f * Vector3::dot(s, h);

	if (u < 0.0 || u > 1.0) {
		return unique_ptr<Vector3>(nullptr);
	}

	q = Vector3::cross(s, edge1);
	v = f * Vector3::dot(rayDirection, q);

	if (v < 0.0 || u + v > 1.0) {
		return unique_ptr<Vector3>(nullptr);
	}
	double t = f * Vector3::dot(edge2, q);
	if (t > EPSILON) {
		Vector3* result = new Vector3;
		*result = rayOrigin + rayDirection * t;
		return unique_ptr<Vector3>(result);
	} else {
		return unique_ptr<Vector3>(nullptr);
	}
}

bool intersectAABB(const Ray& ray, const Vector3& min, const Vector3& max, float& tmin, float& tmax) {
	const Vector3& origin = ray.getOrigin();
	const Vector3& direction = ray.getDirection();

	float txmin = (min.x - origin.x) / direction.x;
	float txmax = (max.x - origin.x) / direction.x;

	if (txmin > txmax) {
		swap(txmin, txmax);
	}

	float tymin = (min.y - origin.y) / direction.y;
	float tymax = (max.y - origin.y) / direction.y;

	if (tymin > tymax) {
		swap(tymin, tymax);
	}

	if ((txmin > tymax) || (tymin > txmax)) {
		return false;
	}

	if (tymin > txmin) {
		txmin = tymin;
	}

	if (tymax < txmax) {
		txmax = tymax;
	}

	float tzmin = (min.z - origin.z) / direction.z;
	float tzmax = (max.z - origin.z) / direction.z;

	if (tzmin > tzmax) {
		swap(tzmin, tzmax);
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

	tmin = txmin;
	tmax = txmax;

	return true;
}

bool intersectAABB(const Ray& ray, const Vector3& min, const Vector3& max) {
	float tmin = numeric_limits<float>::infinity();
	float tmax = -numeric_limits<float>::infinity();
	return intersectAABB(ray, min, max, tmin, tmax);
}

bool intersectAABB(const Ray& ray, const AABB& aabb, float& tmin, float& tmax) {
	return intersectAABB(ray, aabb.getMin(), aabb.getMax(), tmin, tmax);
}

bool intersectAABB(const Ray& ray, const AABB& aabb) {
	return intersectAABB(ray, aabb.getMin(), aabb.getMax());
}

