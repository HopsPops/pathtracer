#pragma once

#include <mesh.hpp>
#include <shader.hpp>
#include "aabb.hpp"
#include <vector>
#include <string>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int loadTexture(const char *path, const string &directory, bool gamma = false);

class Model {
	public:
		vector<Texture> texturesLoaded;
		vector<Mesh> meshes;
		string directory;
		bool gammaCorrection;
		AABB bounding;

		Model(string const &path, bool gamma = false) :
				gammaCorrection(gamma) {
			loadModel(path);
		}

	private:
		void loadModel(string const &path) {
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

			if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
				cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
				return;
			}

			directory = path.substr(0, path.find_last_of('/'));

			processNode(scene->mRootNode, scene);

			bounding = boundingBox();
			printf("loaded model: %s\n", path.c_str());
			printf("meshes:\n");
			int i = 0;
			for (Mesh m : meshes) {
				printf("\tmesh %d\n", i++);
//				printf("\t\tvertices %d\n", m.vertices.size());
				printf("\t\tpositions %d\n", m.positions.size());
				printf("\t\normals %d\n", m.normals.size());
				printf("\t\ttexcoords %d\n", m.texcoords.size());
				printf("\t\tindices %d\n", m.indices.size());
				printf("\t\ttextures %d\n", m.textures.size());
			}
		}

		AABB boundingBox() {
			Vec3 minimum(100000.0f, 100000.0f, 100000.0f);
			Vec3 maximum(-100000.0f, -100000.0f, -100000.0f);
			for (Mesh m : meshes) {
				for (Vec3 v : m.positions) {
					minimum.x = min(minimum.x, v.x);
					minimum.y = min(minimum.y, v.y);
					minimum.z = min(minimum.z, v.z);
					maximum.x = max(maximum.x, v.x);
					maximum.y = max(maximum.y, v.y);
					maximum.z = max(maximum.z, v.z);
				}
//				for (Vertex v : m.vertices) {
//					minimum.x = min(minimum.x, v.position.x);
//					minimum.y = min(minimum.y, v.position.y);
//					minimum.z = min(minimum.z, v.position.z);
//
//					maximum.x = max(maximum.x, v.position.x);
//					maximum.y = max(maximum.y, v.position.y);
//					maximum.z = max(maximum.z, v.position.z);
//				}
			}
			return AABB(minimum, maximum);
		}

		void processNode(aiNode *node, const aiScene *scene) {
			for (unsigned int i = 0; i < node->mNumMeshes; i++) {
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			for (unsigned int i = 0; i < node->mNumChildren; i++) {
				processNode(node->mChildren[i], scene);
			}

		}

		Mesh processMesh(aiMesh *m, const aiScene *scene) {
//			vector<Vertex> vertices;
			vector<unsigned int> indices;
			vector<Texture> textures;

			vector<Vec3> positions { };
			vector<Vec2> texcoords { };
			vector<Vec3> normals { };

			for (unsigned int i = 0; i < m->mNumVertices; i++) {
//				Vertex vertex;
//				Vec3 vector;

//				vector.x = m->mVertices[i].x;
//				vector.y = m->mVertices[i].y;
//				vector.z = m->mVertices[i].z;
//				vertex.position = vector;
				positions.push_back( { m->mVertices[i].x, m->mVertices[i].y, m->mVertices[i].z });

				if (m->mNormals) {
//					vector.x = m->mNormals[i].x;
//					vector.y = m->mNormals[i].y;
//					vector.z = m->mNormals[i].z;
//					vertex.normals = vector;
					normals.push_back( { m->mNormals[i].x, m->mNormals[i].y, m->mNormals[i].z });
				} else {
//					vertex.normals = Vec3(0.0f, 0.0f, 0.0f);
					normals.push_back( { 0.0f, 0.0f, 0.0f });
				}
				if (m->mTextureCoords[0]) {
					Vec2 vec;
					vec.x = m->mTextureCoords[0][i].x;
					vec.y = m->mTextureCoords[0][i].y;
//					vertex.tex = vec;
					texcoords.push_back( { m->mTextureCoords[0][i].x, m->mTextureCoords[0][i].y });
				} else {
//					vertex.tex = Vec2(0.0f, 0.0f);
					texcoords.push_back( { 0.0f, 0.0f });
				}
//				vertices.push_back(vertex);
			}
			for (unsigned int i = 0; i < m->mNumFaces; i++) {
				aiFace face = m->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++) {
					indices.push_back(face.mIndices[j]);
				}
			}
			aiMaterial* material = scene->mMaterials[m->mMaterialIndex];

			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
			Mesh mesh(positions, texcoords, normals, indices, textures);
//			mesh.positions = positions;
//			mesh.texcoords = texcoords;
//			mesh.normals = normals;

			return mesh;
		}

		vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
			vector<Texture> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
				aiString str;
				mat->GetTexture(type, i, &str);

				bool skip = false;
				for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
					if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
							{
						textures.push_back(texturesLoaded[j]);
						skip = true;
						break;
					}
				}
				if (!skip) {
					Texture texture;
					texture.id = loadTexture(str.C_Str(), this->directory);
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);
					texturesLoaded.push_back(texture);
				}
			}
			return textures;
		}
};

unsigned int loadTexture(const char *path, const string &directory, bool gamma) {
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		} else if (nrComponents == 3) {
			format = GL_RGB;
		} else if (nrComponents == 4) {
			format = GL_RGBA;
		} else {
			//TODO
			format = GL_RGB;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

