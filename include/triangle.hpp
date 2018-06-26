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

		Vector3 middle();
		Vertex operator[](int i) const;

};

typedef std::vector<const Triangle*> Triangles;

double trianglesMedian(const Triangles&, int);
