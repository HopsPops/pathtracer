#pragma once
#include "math.hpp"
#include "vertex.hpp"
#include <vector>

class Triangle {
	public:
		unsigned int id;
		Vertex v1;
		Vertex v2;
		Vertex v3;
//		Matrix4x4* p = nullptr;
		Vector3 tangent1 {};
		Vector3 tangent2 {};

		Vector3 middle();
		const Vertex& operator[](int i) const;

		bool isCounterclockwise() const {
			Vector3 v1 = this->v1.position;
			Vector3 v2 = this->v2.position;
			Vector3 v3 = this->v3.position;

			double det = 1.0 / 2.0 * Vector3::cross(v2 - v1, v3 - v1).length();
			return det >= 0;
		}

		Vector3 normal() const {
			const Vector3& u = tangent1;
			const Vector3& v = tangent2;

			Vector3 n { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };
			n.normalize();
			return n;
		}
};

typedef std::vector<const Triangle*> Triangles;

double trianglesMedian(const Triangles&, int);
