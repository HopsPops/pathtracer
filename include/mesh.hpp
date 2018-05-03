#pragma once

#include <aabb.hpp>
#include <types.hpp>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

struct Vertex {
		Vec3 position;
		Vec3 normals;
		Vec2 tex;
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
//		vector<Vertex> vertices;
		vector<Vec3> positions;
		vector<Vec2> texcoords;
		vector<Vec3> normals;
		vector<unsigned int> indices;
		vector<Texture> textures;
		AABB bounding;

		Mesh(vector<Vec3> positions, vector<Vec2> texcoords, vector<Vec3> normals, vector<unsigned int> indices, vector<Texture> textures) {
//			this->vertices = vertices;
			this->positions = positions;
			this->texcoords = texcoords;
			this->normals = normals;
			this->indices = indices;
			this->textures = textures;

			bounding = boundingBox();
		}

		Mesh() {
//			vertices = vector<Vertex>();
			positions = vector<Vec3>();
			texcoords = vector<Vec2>();
			normals = vector<Vec3>();
			indices = vector<unsigned int>();
			textures = vector<Texture>();
		}

	private:
		AABB boundingBox() {
			Vec3 minimum(100000.0f, 100000.0f, 100000.0f);
			Vec3 maximum(-100000.0f, -100000.0f, -100000.0f);
			for (Vec3 v : positions) {
				minimum.x = min(minimum.x, v.x);
				minimum.y = min(minimum.y, v.y);
				minimum.z = min(minimum.z, v.z);
				maximum.x = max(maximum.x, v.x);
				maximum.y = max(maximum.y, v.y);
				maximum.z = max(maximum.z, v.z);
//				minimum.x = min(minimum.x, v.position.x);
//				minimum.y = min(minimum.y, v.position.y);
//				minimum.z = min(minimum.z, v.position.z);
//				maximum.x = max(maximum.x, v.position.x);
//				maximum.y = max(maximum.y, v.position.y);
//				maximum.z = max(maximum.z, v.position.z);
			}
			return AABB(minimum, maximum);
		}

};

