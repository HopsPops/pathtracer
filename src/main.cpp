#include "shader.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include "renderer.hpp"

#include "lodepng.h"

#include <chrono>
#include <thread>

#include <string>
#include <vector>
#include <string.h>
#include <atomic>

#include <iostream>

#include "window.hpp"
#include "config.hpp"

#include "3dtree.hpp"

using namespace std;

void raytrace(Model*);

unique_ptr<Window> window;
Renderer* renderer;
OpenglModel* om;
float zOffset = 10.0f;
Model* m = nullptr;
Config config;
Camera camera;

void removeCurrentModel() {
	if (m != nullptr) {
		delete m;
		m = nullptr;
	}
	if (om != nullptr) {
		delete om;
		m = nullptr;
	}
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	zOffset -= yoffset;
}

Triangles modelToTriangles(Model* model, Matrix4x4 transformation) {
	vector<Triangle> triangles { };
	for (Mesh mesh : model->meshes) {
		for (unsigned int i = 0; i < mesh.indices.size();) {
			Triangle triangle;
			triangle.v1 = {Matrix4x4::multiply(transformation, mesh.positions[mesh.indices[i]], 1.0f), mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
			triangle.v2 = {Matrix4x4::multiply(transformation, mesh.positions[mesh.indices[i]], 1.0f), mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
			triangle.v3 = {Matrix4x4::multiply(transformation, mesh.positions[mesh.indices[i]], 1.0f), mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
			triangles.push_back(triangle);
		}
	}
	return triangles;
}

Triangles modelToTriangles(Model* model) {
	vector<Triangle> triangles { };
	for (Mesh mesh : model->meshes) {
		for (unsigned int i = 0; i < mesh.indices.size();) {
			Triangle triangle;
			triangle.v1 = {mesh.positions[mesh.indices[i]], mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
			triangle.v2 = {mesh.positions[mesh.indices[i]], mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
			triangle.v3 = {mesh.positions[mesh.indices[i]], mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
			triangles.push_back(triangle);
		}
	}
	return triangles;
}

void dropCallback(GLFWwindow* window, int count, const char** paths) {
	string path = string(paths[0]);
	replace(path.begin(), path.end(), '\\', '/');
	cout << path << endl;

	removeCurrentModel();

	m = new Model(path, true);
	om = new OpenglModel(*m);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		removeCurrentModel();
	} else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		renderer->toggleMsaa();
	} else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		renderer->toggleAmbientLight();
	} else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		renderer->toggleSpecularLight();
	} else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		renderer->toggleDiffuseLight();
	} else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
	} else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		raytrace(m);
	} else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		GLubyte* pixels = new GLubyte[4 * window->width() * window->height()] { 0 };
		glReadPixels(0, 0, window->width(), window->height(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		for (int i = 0; i < (window->height() / 2); i++) {
			std::swap_ranges(pixels + window->width() * (i) * 4, pixels + (window->width() * (i + 1) * 4), pixels + ((window->height() - 1 - i) * window->width()) * 4);
		}

		unsigned error = lodepng::encode(string(config.output), pixels, window->width(), window->height());
		cout << error << " " << config.output << endl;
	}
}

void updateWindowTitle() {
	string title("modelv [");
	title += renderer->isAmbientLight() ? "a" : "-";
	title += renderer->isSpecularLight() ? "s" : "-";
	title += renderer->isDiffuseLight() ? "d" : "-";
	title += renderer->isMsaa() ? "m" : "-";
	title += "]";
	title += " w:h=";
	title += std::to_string(window->width());
	title += ":";
	title += std::to_string(window->height());
	window->setTitle(title.c_str());
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
	updateWindowTitle();
}

Matrix4x4 transformations() {
	Matrix4x4 model = Matrix4x4::identityMatrix();
	Matrix4x4 translate = Matrix4x4::translationMatrix(Vector3(-(m->bounding.getMax().x + m->bounding.getMin().x) / 2.0f, -(m->bounding.getMax().y + m->bounding.getMin().z) / 2.0f, -(m->bounding.getMax().y + m->bounding.getMin().z) / 2.0f - 3.0f - zOffset));

	return model * translate;
}

class RaytraceStatistics {
	public:
		std::atomic<int> tests;
		std::atomic<int> intersections;
		std::atomic<int> rays;
		RaytraceStatistics() noexcept :tests(0), intersections(0), rays(0) {};
	};

void rt(Model* model, KdTree* tree, unsigned char* data, int h, int n, RaytraceStatistics* stats) {
	const int w = window->width();

//	Matrix4x4 modelMatrix = transformations();

	for (int y = h; y < n; y++) {
		for (int x = 0; x < w; x++) {
			Ray ray = Ray::createRay(camera, x, y, w, window->height(), window->aspectRatio());
			stats->rays++;

//			if (!intersectAABB(ray, Matrix4x4::multiply(modelMatrix, model->bounding.getMin(), 1.0f), Matrix4x4::multiply(modelMatrix, model->bounding.getMax(), 1.0f))) {
//				continue;
//			}

			float depth = INFINITY;

			//11974873 - 128
			//3637202 - 32
			vector<const KdTree*> trees { };
			traverse(ray, tree, &trees);
			for (const KdTree* t : trees) {
				for (Triangle triangle : t->triangles) {
					Vector3 v1 = triangle.v1.position;
					Vector3 v2 = triangle.v2.position;
					Vector3 v3 = triangle.v3.position;

					stats->tests++;
					Vector3 intersection(0.0, 0.0, 0.0);

					if (!intersectTriangle(ray, v1, v2, v3, intersection)) {
						continue;
					} else {
						depth = 1.0f;
						goto escape;
					}
				}
			}

//			for (Mesh &mesh : model->meshes) {
//				if (!intersectAABB(ray, Matrix4x4::multiply(modelMatrix, mesh.bounding.getMin(), 1.0f), Matrix4x4::multiply(modelMatrix, mesh.bounding.getMax(), 1.0f))) {
//					continue;
//				}
//
//				Vector3 intersection(0.0, 0.0, 0.0);
//				for (unsigned int i = 0; i < mesh.positions.size();) {
//					Vector3 v1 = Matrix4x4::multiply(modelMatrix, mesh.positions[i++], 1.0f);
//					Vector3 v2 = Matrix4x4::multiply(modelMatrix, mesh.positions[i++], 1.0f);
//					Vector3 v3 = Matrix4x4::multiply(modelMatrix, mesh.positions[i++], 1.0f);
//
//					stats->tests++;
//
//					if (!intersectTriangle(ray, v1, v2, v3, intersection)) {
//						continue;
//					}

//					double dist = glm::distance(ray.getOrigin(), intersection);
//
//					if (dist >= depth) {
//						continue;
//					}
//					depth = dist;
//					depth = 1.0; //póki co i tak sprawdzamy tylko czy natrafia na obiekt wiêc nie musimy robic depth-testu
//					stats->intersections++;
//					goto escape;
			// wychodzimy z podwójnej pêtli
//				}
//			}
			escape:

			if (depth < INFINITY) {
				data[(x + y * w) * 3 + 0] = 255;
				data[(x + y * w) * 3 + 1] = 0;
				data[(x + y * w) * 3 + 2] = 0;
			}
		}
//		cout << " " << y << endl;
	}
}

void raytrace(Model* model) {
	clock_t begin = clock();

	unsigned char data[window->width() * window->height() * 3] = { 0 };
	const int q = std::thread::hardware_concurrency();
	vector<thread> threads;
	int h = window->height();

	RaytraceStatistics* stats = new RaytraceStatistics();
	stats->tests = 0;
	stats->intersections = 0;

	KdTree tree(modelToTriangles(model, transformations()), X);
	printf("depth %u\n", depth(&tree));
	printf("triangles %u\n", countTriangles(&tree));
	printf("leafs %u\n", countLeafs(&tree));
	printf("average %lf\n", averageTrianglesPerLeaf(&tree));

	for (int i = 0; i < q; i++) {
		int from = h / q * i;
		int to = from + h / q;
//		cout << from << " " << to << endl;
		threads.push_back(thread(rt, model, &tree, &data[0], from, to, stats));
	}

	for (auto& th : threads) {
		th.join();
	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << "seconds: " << elapsed_secs << endl;
	cout << "total rays: " << stats->rays << endl;
	cout << "total tests: " << stats->tests << endl;
	cout << "total intersections: " << stats->intersections << endl;

	delete stats;

	unsigned error = lodepng::encode(string(config.output), data, window->width(), window->height(), LCT_RGB);
	cout << error << " " << config.output << endl;
}

void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
//		Ray r = Ray::createRay(camera, window->cursorX(), window->cursorY(), window->width(), window->height(), window->aspectRatio());
//		r.print(cout);
//		Matrix4x4 mvp = camera.view() * transformations();

//		Vector3 v1 = multiply(mvp, m->meshes[0].vertices[0].position);
//		Vector3 v2 = multiply(mvp, m->meshes[0].vertices[1].position);
//		Vector3 v3 = multiply(mvp, m->meshes[0].vertices[2].position);

//		cout << "{{" << v1.x << ", " << v1.y << ", " << v1.z << "}, {" << v2.x << ", " << v2.y << ", " << v2.z << "}, {" << v3.x << ", " << v3.y << ", " << v3.z << "}}\n";
//		cout << std::flush;
//		Vector3 result();
	}
}

int main(int argc, char** argv) {
	if (!loadConfig(config)) {
		cout << "unable to load config file\n" << endl;
	}
	config.print(cout);

	camera.setPosition(config.viewPoint);
	camera.lookAt(config.lookAt);
	camera.setUp(config.up);

	window = Window::createWindow();
	window->setFramebufferSizeCallback(framebufferSizeCallback);
	window->setScrollCallback(scrollCallback);
	window->setDropCallback(dropCallback);
	window->setKeyCallback(keyCallback);
	window->setMouseButtonCallback(mouseButtonCallback);
	window->setWindowSizeCallback(windowSizeCallback);

	renderer = new Renderer(window.get(), "shaders/vertex.glsl", "shaders/fragment.glsl");
	updateWindowTitle();

	m = new Model(string(config.input), true);
	om = new OpenglModel(*m);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!window->shouldClose()) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;

		if (deltaTime > 1.0f / 60.0f) {
			lastFrame = currentFrame;

			renderer->clearColor(0.75f, 0.7f, 0.7f);
			renderer->clearColorBuffer();
			renderer->clearDepthBuffer();

			if (m) {
				renderer->draw(*om, camera, transformations());
				renderer->drawLine(Matrix4x4::multiply(transformations(), m->bounding.getMin(), 1.0f), Matrix4x4::multiply(transformations(), m->bounding.getMax(), 1.0f), camera, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
			}

			window->swapBuffers();
		} else {
			this_thread::sleep_for(chrono::milliseconds(2));
			updateWindowTitle();
		}
		window->pollEvents();
	}

	window->terminate();
	return 0;
}

