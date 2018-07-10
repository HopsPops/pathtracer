#pragma once

#include <aabb.hpp>
#include <math.hpp>
#include <vector>

using namespace std;

class Mesh {
	public:
		vector<Vector3> positions { };
		vector<Vector2> texcoords { };
		vector<Vector3> normals { };
		vector<unsigned int> indices { };
		Material material { };
		AABB bounding;

		Mesh(vector<Vector3> positions, vector<Vector2> texcoords, vector<Vector3> normals, vector<unsigned int> indices, Material material);
		Mesh();
};

