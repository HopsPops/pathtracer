#pragma once

#include <aabb.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include "math.hpp"

using namespace std;

struct Vertex {
		Vector3 position;
		Vector3 normals;
		Vector2 tex;
};

class Triangle {
	public:
		Vertex v1;
		Vertex v2;
		Vertex v3;

};

struct Texture {
		unsigned int id;
		string type;
		string path;
};

class Mesh {
	public:
		vector<Vector3> positions;
		vector<Vector2> texcoords;
		vector<Vector3> normals;
		vector<unsigned int> indices;
		vector<Texture> textures;
		AABB bounding;

		Mesh(vector<Vector3> positions, vector<Vector2> texcoords, vector<Vector3> normals, vector<unsigned int> indices, vector<Texture> textures) {
			this->positions = positions;
			this->texcoords = texcoords;
			this->normals = normals;
			this->indices = indices;
			this->textures = textures;

			bounding = boundingBox();
		}

		Mesh() {
			positions = vector<Vector3>();
			texcoords = vector<Vector2>();
			normals = vector<Vector3>();
			indices = vector<unsigned int>();
			textures = vector<Texture>();
		}

	private:
		AABB boundingBox() {
			Vector3 minimum(100000.0f, 100000.0f, 100000.0f);
			Vector3 maximum(-100000.0f, -100000.0f, -100000.0f);
			for (Vector3 v : positions) {
				minimum.x = min(minimum.x, v.x);
				minimum.y = min(minimum.y, v.y);
				minimum.z = min(minimum.z, v.z);
				maximum.x = max(maximum.x, v.x);
				maximum.y = max(maximum.y, v.y);
				maximum.z = max(maximum.z, v.z);
			}
			return AABB(minimum, maximum);
		}

};

