#include "intersection.hpp"
#include <triangle.hpp>
#include <vertex.hpp>
#include <ray.hpp>


std::unique_ptr<Vector3> intersectTriangle(const Ray& ray, const Vector3& vertex0, const Vector3& vertex1, const Vector3& vertex2) {
	const double EPSILON = 0.00000000001;

	Vector3 rayVector = ray.getDirection();
	Vector3 rayOrigin = ray.getOrigin();

	Vector3 edge1, edge2, h, s, q;

	double a = 0.0, f = 0.0, u = 0.0, v = 0.0;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;

	h = Vector3::cross(rayVector, edge2);
	a = Vector3::dot(edge1, h);

	if (a > -EPSILON && a < EPSILON) {
		return std::unique_ptr<Vector3>(nullptr);
	}
	f = 1.0 / a;
	s = rayOrigin - vertex0;
	u = f * Vector3::dot(s, h);

	if (u < 0.0 || u > 1.0) {
		return std::unique_ptr<Vector3>(nullptr);
	}

	q = Vector3::cross(s, edge1);
	v = f * Vector3::dot(rayVector, q);

	if (v < 0.0 || u + v > 1.0) {
		return std::unique_ptr<Vector3>(nullptr);
	}
	double t = f * Vector3::dot(edge2, q);
	if (t > EPSILON) {
//		outIntersectionPoint = rayOrigin + rayVector * t;
		Vector3* result = new Vector3;
		*result = rayOrigin + rayVector * t;
		return std::unique_ptr<Vector3>(result);
	} else {
		return std::unique_ptr<Vector3>(nullptr);
	}
}

std::unique_ptr<Vector3> intersectTriangle(const Ray& ray, const Triangle& t) {
	return intersectTriangle(ray, t.v1.position, t.v2.position, t.v3.position);
}

bool intersectAABB(const Ray& ray, const Vector3& min, const Vector3& max) {

	double originX = ray.getOrigin().x;
	double originY = ray.getOrigin().y;
	double originZ = ray.getOrigin().z;

	if (originX <= max.x &&
			originX >= min.x &&
			originY <= max.y &&
			originY >= min.y &&
			originZ <= max.z &&
			originZ >= min.z) {
		return true;
	}

	double txmin = (min.x - ray.getOrigin().x) / ray.getDirection().x;
	double txmax = (max.x - ray.getOrigin().x) / ray.getDirection().x;

	if (txmin > txmax) {
		float temp = txmin;
		txmin = txmax;
		txmax = temp;
	}

	double tymin = (min.y - ray.getOrigin().y) / ray.getDirection().y;
	double tymax = (max.y - ray.getOrigin().y) / ray.getDirection().y;

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

	double tzmin = (min.z - ray.getOrigin().z) / ray.getDirection().z;
	double tzmax = (max.z - ray.getOrigin().z) / ray.getDirection().z;

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

