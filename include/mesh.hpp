#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include "types.hpp"

using namespace std;

struct vertex {
		vec3 position;
		vec3 normals;
		vec2 tex;
};

struct texture {
		unsigned int id;
		string type;
		string path;
};

struct box {
		vec3 min;
		vec3 max;
};

class mesh {
	public:
		vector<vertex> vertices;
		vector<unsigned int> indices;
		vector<texture> textures;
		unsigned int vao;
		box bounding;

		mesh(vector<vertex> vertices, vector<unsigned int> indices, vector<texture> textures) {
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			setupMesh();
			bounding = boundingBox();
		}

	private:
		unsigned int vbo, ebo;

		void setupMesh() {
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, position));

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, normals));

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, tex));

			glBindVertexArray(0);
		}

		box boundingBox() {
			vec3 minimum(100000.0f, 100000.0f, 100000.0f);
			vec3 maximum(-100000.0f, -100000.0f, -100000.0f);
			for (vertex v : vertices) {
				minimum.x = min(minimum.x, v.position.x);
				minimum.y = min(minimum.y, v.position.y);
				minimum.z = min(minimum.z, v.position.z);

				maximum.x = max(maximum.x, v.position.x);
				maximum.y = max(maximum.y, v.position.y);
				maximum.z = max(maximum.z, v.position.z);
			}
			return box { minimum, maximum };
		}

};
