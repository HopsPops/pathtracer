#pragma once

#include <math.hpp>
#include <vertex.hpp>
#include <vector>

class Triangle {
	public:
		long id;
		Vertex v1 { };
		Vertex v2 { };
		Vertex v3 { };
		Vector3 tangent1 { };
		Vector3 tangent2 { };
		const Material* material { };

		Vector3 middle() const;
		const Vertex& operator[](int i) const;

		Triangle(long id, Vertex v1, Vertex v2, Vertex v3, Vector3 t1, Vector3 t2, const Material* mat);

		bool isCounterclockwise() const;

		float area() const;

		Vector3 normal() const;

		Vector3 randomPointOnTriangle() const;

		bool isLight() const;

		float minAxis(int axis) const;
		float maxAxis(int axis) const;

};

typedef std::vector<const Triangle*> Triangles;

float trianglesMedian(const Triangles&, int);

Triangles findLights(const Triangles& triangles);
