#pragma once

#include <aabb.hpp>
#include <assimp/material.h>
#include <math.hpp>
#include <mesh.hpp>
#include <triangle.hpp>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int loadTexture(const char *path, const string &directory, bool gamma = false);

class Model {
	public:
		vector<Mesh> meshes;
		string directory;
		AABB bounding;

		Model(string const &path);

	private:
		void loadModel(string const &path);

		AABB boundingBox();

		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *m, const aiScene *scene);

};

Triangles* modelToTriangles(Model* model, Matrix4x4 transformation = Matrix4x4::identityMatrix());
