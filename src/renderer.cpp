#include <renderer.hpp>
#include <camera.hpp>
#include <glad.h>
#include <mesh.hpp>
#include <stdio.h>
#include <triangle.hpp>
#include <vertex.hpp>

OpenglMesh::OpenglMesh(const Mesh& mesh) {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glGenBuffers(1, &vboPositions);
	glGenBuffers(1, &vboNormals);
	glGenBuffers(1, &vboTexcoords);

	glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
	glBufferData(GL_ARRAY_BUFFER, mesh.positions.size() * sizeof(Vector3), &mesh.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*) 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(Vector3), &mesh.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*) 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
	glBufferData(GL_ARRAY_BUFFER, mesh.texcoords.size() * sizeof(Vector2), &mesh.texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (void*) 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	indices = mesh.indices.size();

	printf("opengl: mesh loaded\n");
	printf("opengl: vbo:%d\n", vbo);
	printf("opengl: vao:%d\n", vao);
	printf("opengl: ebo:%d\n", ebo);
	printf("opengl: indices:%d\n", indices);
}

unsigned int OpenglMesh::getVBO() const {
	return vbo;
}

unsigned int OpenglMesh::getVAO() const {
	return vao;
}

unsigned int OpenglMesh::getEBO() const {
	return ebo;
}

unsigned int OpenglMesh::getVertices() const {
	return vertices;
}

unsigned int OpenglMesh::getIndices() const {
	return indices;
}

OpenglModel::OpenglModel(const Model& m) {
	for (Mesh mesh : m.meshes) {
		meshes.push_back(OpenglMesh(mesh));
	}
}

const std::vector<OpenglMesh>& OpenglModel::getMeshes() const {
	return meshes;
}

Renderer::Renderer(Window* window, const char* vertexPath, const char* fragmentPath) {
	this->window = window;
	lineShader = Shader::loadFromFile("shaders/vline.glsl", "shaders/fline.glsl");
	defaultShader = Shader::loadFromFile(vertexPath, fragmentPath);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(lineWidth);
	glPointSize(5.0f);

	unsigned int vao = 0;
	glGenVertexArrays(1, &vao);
}
;

void Renderer::clearColor(const Vector3& color) {
	glClearColor(color.x, color.y, color.z, 1.0f);
}

void Renderer::clearColor(int x, int y, int z) {
	glClearColor(x / 255.0f, y / 255.0f, z / 255.0f, 1.0f);
}

void Renderer::clearColor(float x, float y, float z) {
	glClearColor(x, y, z, 1.0f);
}

void Renderer::setViewport(int width, int height) {
	glViewport(0, 0, width, height);
}

void Renderer::setLineWidth(float width) {
	if (this->lineWidth == width) {
		return;
	}
	this->lineWidth = width;
	glLineWidth(width);
}

void Renderer::clearColorBuffer() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::clearDepthBuffer() {
	glClear(GL_DEPTH_BUFFER_BIT);
}

bool Renderer::isMsaa() const {
	return msaa;
}

bool Renderer::isAmbientLight() const {
	return ambient;
}

bool Renderer::isSpecularLight() const {
	return specular;
}

bool Renderer::isDiffuseLight() const {
	return diffuse;
}

void Renderer::toggleAmbientLight() {
	ambient = !ambient;
	defaultShader->set("ambientLight", ambient);
}

void Renderer::toggleSpecularLight() {
	specular = !specular;
	defaultShader->set("specularLight", specular);
}

void Renderer::toggleDiffuseLight() {
	diffuse = !diffuse;
	defaultShader->set("diffuseLight", diffuse);
}

void Renderer::toggleMsaa() {
	msaa = !msaa;
	if (msaa) {
		glEnable(GL_MULTISAMPLE);
	} else {
		glDisable(GL_MULTISAMPLE);
	}
}

void Renderer::drawLine(Vector3 v1, Vector3 v2, const Camera& camera, Vector4 color, float lineWidth) {
	setLineWidth(lineWidth);
	lineShader->use();
	lineShader->set("color", color);
	lineShader->set("view", camera.view());
	lineShader->set("projection", camera.perspective(window->aspectRatio()));

	GLfloat buffer[] = { v1.x, v1.y, v1.z, v2.x, v2.y, v2.z };

	unsigned int vao = 0;
	unsigned int vbo = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Renderer::drawTriangle(const Triangle& triangle, const Camera& camera, Vector4 color, float lineWidth) {
	drawLine(triangle.v1.position, triangle.v2.position, camera, color, lineWidth);
	drawLine(triangle.v2.position, triangle.v3.position, camera, color, lineWidth);
	drawLine(triangle.v3.position, triangle.v1.position, camera, color, lineWidth);
}

void Renderer::drawPoint(const Vector3& point, const Camera& camera, const Vector4& color) {
	lineShader->use();
	lineShader->set("color", color);
	lineShader->set("view", camera.view());
	lineShader->set("projection", camera.perspective(window->aspectRatio()));

	GLfloat buffer[] = { point.x, point.y, point.z };

	unsigned int vao = 0;
	unsigned int vbo = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_POINTS, 0, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Renderer::draw(const OpenglMesh& m, const Camera& camera, const Matrix4x4& model) {
	defaultShader->use();
	defaultShader->set("ambientColor", Vector3 { 1.0f, 1.0f, 1.0f });
	defaultShader->set("lightPosition", Vector3 { 2.0f, 2.0f, 0.0f });
	defaultShader->set("cameraPosition", Vector3 { 0.0f, 0.0f, 0.0f });

	defaultShader->set("colorOverride", Vector4 { 1.0f, 1.0f, 1.0f, 0.0f });
	defaultShader->set("ambientLight", ambient);
	defaultShader->set("specularLight", specular);
	defaultShader->set("diffuseLight", diffuse);

	defaultShader->set("textures", false);

	defaultShader->set("projection", camera.perspective(window->aspectRatio()));
	defaultShader->set("view", camera.view());
	defaultShader->set("model", model);
	glBindVertexArray(m.getVAO());
	glDrawElements(GL_TRIANGLES, m.getIndices(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Renderer::draw(const OpenglModel& m, const Camera& camera, const Matrix4x4& model) {
	for (const OpenglMesh& mesh : m.getMeshes()) {
		draw(mesh, camera, model);
	}
}

void Renderer::drawKdTreeLeafs(const KdTree* tree, const Camera& camera, float lineWidth) {
	int d = depth(tree);
	drawKdTreeLeafs(tree, camera, 0, d, lineWidth);
}

void Renderer::drawKdTreeLeafs(const KdTree* tree, const Camera& camera, int level, int depth, float lineWidth) {
	if (tree == nullptr) {
		return;
	}
	if (tree->isLeaf()) {
		if ((tree->getAxis() % 3) == 0) {
			drawAABB(tree->getAABB(), camera, Vector4 { 1.0f, 0.0f, 0.0f, 1.0f - (float) (level / 1.5f / (float) depth) }, lineWidth);
		} else if ((tree->getAxis() % 3) == 1) {
			drawAABB(tree->getAABB(), camera, Vector4 { 0.0f, 1.0f, 0.0f, 1.0f - (float) (level / 1.5f / (float) depth) }, lineWidth);
		} else {
			drawAABB(tree->getAABB(), camera, Vector4 { 0.0f, 0.0f, 1.0f, 1.0f - (float) (level / 1.5f / (float) depth) }, lineWidth);
		}
	}
	drawKdTreeLeafs(tree->getLeft(), camera, level + 1, depth, lineWidth);
	drawKdTreeLeafs(tree->getRight(), camera, level + 1, depth, lineWidth);
}

void Renderer::drawAABB(const AABB& aabb, const Camera& camera, const Vector4& color, float lineWidth) {

	Vector3 leftBottomDown = aabb.getMin();
	Vector3 leftTopDown(aabb.getMin().x, aabb.getMin().y, aabb.getMax().z);
	Vector3 rightBottomDown(aabb.getMax().x, aabb.getMin().y, aabb.getMin().z);
	Vector3 rightTopDown(aabb.getMax().x, aabb.getMin().y, aabb.getMax().z);

	Vector3 leftBottomUp = leftBottomDown.copy();
	Vector3 leftTopUp = leftTopDown.copy();
	Vector3 rightBottomUp = rightBottomDown.copy();
	Vector3 rightTopUp = rightTopDown.copy();
	leftBottomUp.y = aabb.getMax().y;
	leftTopUp.y = aabb.getMax().y;
	rightBottomUp.y = aabb.getMax().y;
	rightTopUp.y = aabb.getMax().y;

	vector<Vector3> buffer { };

	buffer.push_back(leftBottomDown);
	buffer.push_back(rightBottomDown);

	buffer.push_back(rightBottomDown);
	buffer.push_back(rightTopDown);

	buffer.push_back(rightTopDown);
	buffer.push_back(leftTopDown);

	buffer.push_back(leftTopDown);
	buffer.push_back(leftBottomDown);

	buffer.push_back(leftBottomUp);
	buffer.push_back(rightBottomUp);

	buffer.push_back(rightBottomUp);
	buffer.push_back(rightTopUp);

	buffer.push_back(rightTopUp);
	buffer.push_back(leftTopUp);

	buffer.push_back(leftTopUp);
	buffer.push_back(leftBottomUp);

	buffer.push_back(leftBottomDown);
	buffer.push_back(leftBottomUp);

	buffer.push_back(leftTopDown);
	buffer.push_back(leftTopUp);

	buffer.push_back(rightBottomDown);
	buffer.push_back(rightBottomUp);

	buffer.push_back(rightTopDown);
	buffer.push_back(rightTopUp);

	lineShader->use();
	lineShader->set("color", color);
	lineShader->set("view", camera.view());
	lineShader->set("projection", camera.perspective(window->aspectRatio()));

	setLineWidth(lineWidth);

	unsigned int vao = 0;
	unsigned int vbo = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * buffer.size(), buffer.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, buffer.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

