#include <assimp/color4.h>
#include <assimp/mesh.h>
#include <assimp/types.h>
#include <glad.h>
#include <model.hpp>
#include <stdio.h>
#include <vertex.hpp>
#include <cstring>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model(string const &path) {
	loadModel(path);
}

void Model::loadModel(string const &path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

	bounding = AABB(meshes[0].positions);
	for (unsigned int i = 1; i < meshes.size(); i++) {
		bounding = AABB(bounding, AABB(meshes[i].positions));
	}

	printf("loaded model: %s\n", path.c_str());
	printf("meshes:\n");
	int i = 0;
	for (Mesh m : meshes) {
		printf("\tmesh %d\n", i++);
		printf("\t\tpositions %d\n", m.positions.size());
		printf("\t\tnormals %d\n", m.normals.size());
		printf("\t\ttexcoords %d\n", m.texcoords.size());
		printf("\t\tindices %d\n", m.indices.size());
	}
}

void Model::processNode(aiNode *node, const aiScene *scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}

}

Mesh Model::processMesh(aiMesh *m, const aiScene *scene) {
	vector<unsigned int> indices;

	vector<Vector3> positions { };
	vector<Vector2> texcoords { };
	vector<Vector3> normals { };

	for (unsigned int i = 0; i < m->mNumVertices; i++) {
		{
			Vector3 position { m->mVertices[i].x, m->mVertices[i].y, m->mVertices[i].z };
			positions.push_back(position);
		}
		if (m->mNormals) {
			Vector3 normal { m->mNormals[i].x, m->mNormals[i].y, m->mNormals[i].z };
			normals.push_back(normal);
		} else {
			normals.push_back(Vector3());
		}
		if (m->mTextureCoords[0]) {
			Vector2 texcoord { m->mTextureCoords[0][i].x, m->mTextureCoords[0][i].y };
			texcoords.push_back(texcoord);
		} else {
			texcoords.push_back(Vector2());
		}
	}
	for (unsigned int i = 0; i < m->mNumFaces; i++) {
		aiFace face = m->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	aiMaterial* assimpMaterial = scene->mMaterials[m->mMaterialIndex];

	aiColor4D colorDiffuse(0.f, 0.f, 0.f, 0.f);
	aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &colorDiffuse);

	aiColor4D colorEmissive(0.f, 0.f, 0.f, 0.f);
	aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_EMISSIVE, &colorEmissive);

	Material material { Vector3 { colorDiffuse.r, colorDiffuse.g, colorDiffuse.b }, Vector3 { colorEmissive.r, colorEmissive.g, colorEmissive.b } };
	Mesh mesh(positions, texcoords, normals, indices, material);

	return mesh;
}

Triangles* modelToTriangles(Model* model, Matrix4x4 transformation) {
	Triangles* triangles = new Triangles;
	long id = 0;
	for (const Mesh& mesh : model->meshes) {
		for (unsigned int i = 0; i < mesh.indices.size();) {
			Vertex va { mesh.positions[mesh.indices[i++]] };
			Vertex vb { mesh.positions[mesh.indices[i++]] };
			Vertex vc { mesh.positions[mesh.indices[i++]] };

			Vector3 a = va.position;
			Vector3 b = vb.position;
			Vector3 c = vc.position;

			Vector3 ab = b - a;
			Vector3 ac = c - a;
			Triangle* triangle = new Triangle(id++, va, vb, vc, ab, ac, &mesh.material);
			triangles->push_back(triangle);
		}
	}
	return triangles;
}
