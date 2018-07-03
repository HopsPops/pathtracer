#pragma once
#include "math.hpp"
#include "vertex.hpp"
#include <vector>
#include <algorithm>

class Triangle {
	public:
		long id;
		Vertex v1;
		Vertex v2;
		Vertex v3;
		Vector3 tangent1 { };
		Vector3 tangent2 { };
		const Material* material {};

		Vector3 middle();
		const Vertex& operator[](int i) const;

		Triangle(long id, Vertex v1, Vertex v2, Vertex v3, Vector3 t1, Vector3 t2, const Material* mat) {
			this->id = id;
			this->v1 = v1;
			this->v2 = v2;
			this->v3 = v3;
			this->tangent1 = t1;
			this->tangent2 = t2;
			this->material = mat;
		}

		bool isCounterclockwise() const {
			Vector3 v1 = this->v1.position;
			Vector3 v2 = this->v2.position;
			Vector3 v3 = this->v3.position;

			double det = 1.0 / 2.0 * Vector3::cross(v2 - v1, v3 - v1).length();
			return det >= 0;
		}

		float area() const {
			float det =  Vector3::cross(tangent1, tangent2).length() / 2.0f;
			return det;
		}

		Vector3 normal() const {
			const Vector3& u = tangent1;
			const Vector3& v = tangent2;

			Vector3 n { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };
			n.normalize();
			return n;
		}

		float minAxis(int axis) const {
			return std::min(v1.position[axis], std::min(v2.position[axis], v3.position[axis]));
		}

		float maxAxis(int axis) const {
			return std::max(v1.position[axis], std::max(v2.position[axis], v3.position[axis]));
		}

};

typedef std::vector<const Triangle*> Triangles;

float trianglesMedian(const Triangles&, int);

