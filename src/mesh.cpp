#include <mesh.hpp>

Mesh::Mesh(vector<Vector3> positions, vector<Vector2> texcoords, vector<Vector3> normals, vector<unsigned int> indices, Material material) {
	this->positions = positions;
	this->texcoords = texcoords;
	this->normals = normals;
	this->indices = indices;
	this->material = material;

	bounding = AABB(positions);
}

Mesh::Mesh() {
}
