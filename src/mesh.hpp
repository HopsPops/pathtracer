#pragma once

#include <string>
#include <vector>

using namespace std;

struct vertex {
		glm::vec3 position;
		glm::vec3 normals;
		glm::vec2 tex;
};

struct texture {
		unsigned int id;
		string type;
		string path;
};

class mesh {
	public:
		vector<vertex> vertices;
		vector<unsigned int> indices;
		vector<texture> textures;
		unsigned int vao;

		mesh(vector<vertex> vertices, vector<unsigned int> indices, vector<texture> textures) {
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			setupMesh();
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
};
