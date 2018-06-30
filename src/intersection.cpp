#include "intersection.hpp"
#include <triangle.hpp>
#include <vertex.hpp>
#include <ray.hpp>

std::unique_ptr<Vector3> intersectTriangle(const Ray& ray, const Vector3& vertex0, const Vector3& vertex1, const Vector3& vertex2) {
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
		Vector3* result = new Vector3;
		*result = rayOrigin + rayVector * t;
		return std::unique_ptr<Vector3>(result);
	} else {
		return std::unique_ptr<Vector3>(nullptr);
	}
}
/*
 std::unique_ptr<Vector3> intersectTriangle(const Ray& ray, const Triangle& triangle) {
 //	Vector4 o = Matrix4x4::multiply(*triangle.p, ray.getOrigin(), 1.0f);
 //	Vector4 d = Matrix4x4::multiply(*triangle.p, ray.getDirection(), 0.0f);
 //
 //	float t = -o.z / d.z;
 //	float b1 = o.x + t * d.x;
 //	float b2 = o.y + t * d.y;
 //	float b3 = 1 - b1 - b2;
 //	if(b1 < 0.0f || b1 > 1.0f || b2 < 0.0f || b2 > 1.0f || b3 < 0.0f || b3 > 1.0f) {
 //		return std::unique_ptr<Vector3>(nullptr);
 //	}
 //
 //	Vector3* result = new Vector3;
 //	*result = (b1 * triangle.v1.position) + (b2 * triangle.v2.position) + (b3 * triangle.v3.position);
 //	return std::unique_ptr<Vector3>(result);
 //	return intersectTriangle(ray, t.v1.position, t.v2.position, t.v3.position);

 const float EPSILON = 0.000001f;

 const Vector3& rayVector = ray.getDirection();
 const Vector3& rayOrigin = ray.getOrigin();

 Vector3 h, s, q;

 float a = 0.0, f = 0.0, u = 0.0, v = 0.0;
 const Vector3& edge1 = triangle.tangent1;
 const Vector3& edge2 = triangle.tangent2;

 h = Vector3::cross(rayVector, edge2);
 a = Vector3::dot(edge1, h);

 if (a > -EPSILON && a < EPSILON) {
 return std::unique_ptr<Vector3>(nullptr);
 }
 f = 1.0 / a;
 s = rayOrigin - triangle.v1.position;
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
 Vector3* result = new Vector3;
 *result = rayOrigin + rayVector * t;
 return std::unique_ptr<Vector3>(result);
 } else {
 return std::unique_ptr<Vector3>(nullptr);
 }
 }
 */

/*
std::unique_ptr<Vector3> intersectTriangle(const Ray& ray, const Triangle& triangle) {
	float x1 = triangle.v1.position.x;
	float y1 = triangle.v1.position.y;
	float z1 = triangle.v1.position.z;

	float x2 = triangle.v2.position.x;
	float y2 = triangle.v2.position.y;
	float z2 = triangle.v2.position.z;

	float x3 = triangle.v3.position.x;
	float y3 = triangle.v3.position.y;
	float z3 = triangle.v3.position.z;

	const Vector3& normal = triangle.normal();

	const Vector3& direction = ray.getDirection();
	float dx = direction.x;
	float dy = direction.y;
	float dz = direction.z;

	const Vector3& origin = ray.getOrigin();
	float ox = origin.x;
	float oy = origin.y;
	float oz = origin.z;

//	float denom = dx * normal.x + dy * normal.y + dz * normal.z;

	//	if (fabs(denom) < 0.0001f) {
	//		return std::unique_ptr<Vector3>(nullptr);
	//	}

	float d = -(dx * ox + dy * oy + dz * oz);
	float t = -(ox * normal.x + oy * normal.y + oz * normal.z + d) / (dx * normal.x + dy * normal.y + dz * normal.z);

	float px = ox + t * dx;
	float py = oy + t * dy;
	float pz = oz + t * dz;
	Vector3 p { px, py, pz };
	Vector3 v0 = triangle.tangent1, v1 = triangle.tangent2, v2 = p - triangle.v1.position;
	float d00 = Vector3::dot(v0, v0);
	float d01 = Vector3::dot(v0, v1);
	float d11 = Vector3::dot(v1, v1);
	float d20 = Vector3::dot(v2, v0);
	float d21 = Vector3::dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;

	float lambda1 = (d11 * d20 - d01 * d21) / denom;
	if (!(lambda1 >= 0 && lambda1 <= 1)) {
		return std::unique_ptr<Vector3>(nullptr);
	}

	float lambda2 = (d00 * d21 - d01 * d20) / denom;
	if (!(lambda2 >= 0 && lambda2 <= 1)) {
		return std::unique_ptr<Vector3>(nullptr);
	}

	float lambda3 = 1.0f - lambda1 - lambda2;
	if (!(lambda3 >= 0 && lambda3 <= 1)) {
		return std::unique_ptr<Vector3>(nullptr);
	}

	float x = x1 * lambda1 + x2 * lambda2 + x3 * lambda3;
	float y = y1 * lambda1 + y2 * lambda2 + y3 * lambda3;
	float z = z1 * lambda1 + z2 * lambda2 + z3 * lambda3;

	return std::unique_ptr<Vector3>(new Vector3 { x, y, z });
}
*/

bool intersectAABB(const Ray& ray, const Vector3& min, const Vector3& max) {
	const Vector3& origin = ray.getOrigin();
	const Vector3& direction = ray.getDirection();

	float txmin = (min.x - origin.x) / direction.x;
	float txmax = (max.x - origin.x) / direction.x;

	if (txmin > txmax) {
		float temp = txmin;
		txmin = txmax;
		txmax = temp;
	}

	float tymin = (min.y - origin.y) / direction.y;
	float tymax = (max.y - origin.y) / direction.y;

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

	float tzmin = (min.z - origin.z) / direction.z;
	float tzmax = (max.z - origin.z) / direction.z;

	if (tzmin > tzmax) {
		float temp = tzmin;
		tzmin = tzmax;
		tzmax = temp;
	}

	if ((txmin > tzmax) || (tzmin > txmax)) {
		return false;
	}
//
//	if (tzmin > txmin) {
//		txmin = tzmin;
//	}
//
//	if (tzmax < txmax) {
//		txmax = tzmax;
//	}

	return true;
}

bool intersectAABB(const Ray& ray, const AABB& aabb) {
	return intersectAABB(ray, aabb.getMin(), aabb.getMax());
}

