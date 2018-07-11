#pragma once

#include <aabb.hpp>
#include <math.hpp>
#include <model.hpp>
#include <memory>
#include <vector>
#include <shader.hpp>
#include <window.hpp>
#include <3dtree.hpp>

class OpenglMesh {
	private:
		unsigned int vbo = 0;
		unsigned int vboPositions = 0;
		unsigned int vboNormals = 0;
		unsigned int vboTexcoords = 0;
		unsigned int vao = 0;
		unsigned int ebo = 0;
		unsigned int vertices = 0;
		unsigned int indices = 0;

	public:
		OpenglMesh(const Mesh& mesh);

		unsigned int getVBO() const;
		unsigned int getVAO() const;
		unsigned int getEBO() const;

		unsigned int getVertices() const;

		unsigned int getIndices() const;

};

class OpenglModel {
	private:
		std::vector<OpenglMesh> meshes;
		public:
		OpenglModel(const Model& m);

		const std::vector<OpenglMesh>& getMeshes() const;
};

class Renderer {
	private:
		Window* window = nullptr;
		bool msaa = true;
		bool ambient = true;
		bool specular = true;
		bool diffuse = true;
		float lineWidth = 1.0f;
		std::unique_ptr<Shader> defaultShader = std::unique_ptr<Shader>();
		std::unique_ptr<Shader> lineShader = std::unique_ptr<Shader>();

	public:
		Renderer(Window* window, const char* vertexPath, const char* fragmentPath);

		void clearColor(int, int, int);
		void clearColor(float, float, float);
		void clearColor(const Vector3&);

		void setViewport(int width, int height);

		void setLineWidth(float width);

		void clearColorBuffer();

		void clearDepthBuffer();

		bool isMsaa() const;
		bool isAmbientLight() const;
		bool isSpecularLight() const;
		bool isDiffuseLight() const;

		void toggleAmbientLight();
		void toggleSpecularLight();
		void toggleDiffuseLight();
		void toggleMsaa();

		void drawLine(Vector3 v1, Vector3 v2, const Camera& camera, Vector4 color, float lineWidth = 1.0f);

		void drawTriangle(const Triangle& triangle, const Camera& camera, Vector4 color, float lineWidth = 1.0f);

		void drawPoint(const Vector3& point, const Camera& camera, const Vector4& color);

		void draw(const OpenglMesh& m, const Camera& camera, const Matrix4x4& model = Matrix4x4::identityMatrix());

		void draw(const OpenglModel& m, const Camera& camera, const Matrix4x4& model = Matrix4x4::identityMatrix());

		void drawKdTreeLeafs(const KdTree* tree, const Camera& camera, float lineWidth = 1.0f);

		void drawKdTreeLeafs(const KdTree* tree, const Camera& camera, int level, int depth, float lineWidth = 1.0f);

		void drawAABB(const AABB& aabb, const Camera& camera, const Vector4& color, float lineWidth = 1.0f);

};

