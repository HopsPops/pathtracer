#include "intersection.hpp"
#include <triangle.hpp>
#include <vertex.hpp>
#include <ray.hpp>


//bool intersectTriangle(const Ray& ray, Vector3 v1, Vector3 v2, Vector3 v3, Vector3& outIntersectionPoint) {
//	float x1 = v1.x;
//	float y1 = v1.y;
//	float z1 = v1.z;
//
//	float x2 = v2.x;
//	float y2 = v2.y;
//	float z2 = v2.z;
//
//	float x3 = v3.x;
//	float y3 = v3.y;
//	float z3 = v3.z;
//
//	float nx = y2 * z1 - y3 * z1 - y1 * z2 + y3 * z2 + y1 * z3 - y2 * z3;
//	float ny = -x2 * z1 + x3 * z1 + x1 * z2 - x3 * z2 - x1 * z3 + x2 * z3;
//	float nz = x2 * y1 - x3 * y1 - x1 * y2 + x3 * y2 + x1 * y3 - x2 * y3;
//
//	Vector3 direction = ray.getDirection();
//	float dx = direction.x;
//	float dy = direction.y;
//	float dz = direction.z;
//
//	Vector3 origin = ray.getOrigin();
//	float ox = origin.x;
//	float oy = origin.y;
//	float oz = origin.z;
//
//	float denom = dx * nx + dy * ny + dz * nz;
//
//	if (abs(denom) > 0.0000001f) {
//		return false;
//	}
//
//	float d = -(dx * ox + dy * oy + dz * oz);
//	float t = -(ox * nx + oy * ny + oz * nz + d) / denom;
//
//	float px = ox + t * dx;
//	float py = oy + t * dy;
//	float pz = oz + t * dz;
//
//	float det = x3 * y2 * z1 - x2 * y3 * z1 - x3 * y1 * z2 + x1 * y3 * z2 + x2 * y1 * z3 - x1 * y2 * z3;
//	float lambda1 = -(-x3 * y2 * pz + x2 * y3 * pz + x3 * py * z2 - px * y3 * z2 - x2 * py * z3 + px * y2 * z3) / det;
//	float lambda2 = -(x3 * y1 * pz - x1 * y3 * pz - x3 * py * z1 + px * y3 * z1 + x1 * py * z3 - px * y1 * z3) / det;
//	float lambda3 = 1.0f - lambda1 - lambda2;
//
//	if (!(lambda1 >= 0.0f && lambda1 <= 1.0f)) {
//		return false;
//	}
//	if (!(lambda2 >= 0.0f && lambda2 <= 1.0f)) {
//		return false;
//	}
//	if (!(lambda3 >= 0.0f && lambda3 <= 1.0f)) {
//		return false;
//	}
//
//	float x = x1 * lambda1 + x2 * lambda2 + x3 * lambda3;
//	float y = y1 * lambda1 + y2 * lambda2 + y3 * lambda3;
//	float z = z1 * lambda1 + z2 * lambda2 + z3 * lambda3;
//
//	Vector3 result { x, y, z };
//	outIntersectionPoint = result;
//	return true;
//}

std::unique_ptr<Vector3> intersectTriangle(const Ray& ray, const Vector3& vertex0, const Vector3& vertex1, const Vector3& vertex2) {
	const double EPSILON = 0.00000000000001;

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

