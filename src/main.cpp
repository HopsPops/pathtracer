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
#include <algorithm>
#include <random>

using namespace std;

void raytrace(Model*);
void loadModel(string);
Vector3 shotRay(const Ray&, const Triangle*, int);

unique_ptr<Window> window;
Renderer* renderer;
OpenglModel* om;
Triangles* modelTriangles;
KdTree* kdTree;
Model* m = nullptr;
Config config;
Camera camera;
bool gui = true;

void removeCurrentModel() {
	if (m != nullptr) {
		delete m;
		m = nullptr;
	}
	if (om != nullptr) {
		delete om;
		om = nullptr;
	}
	if (modelTriangles != nullptr) {
		delete modelTriangles;
		modelTriangles = nullptr;
	}
	if (kdTree != nullptr) {
		delete kdTree;
		kdTree = nullptr;
	}
}

float clamp(float f) {
	if (f > 1.0f) {
		return 1.0f;
	} else {
		return f;
	}
}

Triangles* modelToTriangles(Model* model, Matrix4x4 transformation) {
	Triangles* triangles = new Triangles;
//	Matrix4x4 transposeInverse = transformation.inverse().transpose();
	unsigned int id = 0;
	for (const Mesh& mesh : model->meshes) {
		for (unsigned int i = 0; i < mesh.indices.size();) {
			Triangle* triangle = new Triangle;
			triangle->id = id++;
			triangle->v1 = {mesh.positions[mesh.indices[i]], mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
			triangle->v2 = {mesh.positions[mesh.indices[i]], mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
			triangle->v3 = {mesh.positions[mesh.indices[i]], mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
//			triangle.v1 = {Matrix4x4::multiply(transformation, mesh.positions[mesh.indices[i]], 1.0f), mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
//			triangle.v2 = {Matrix4x4::multiply(transformation, mesh.positions[mesh.indices[i]], 1.0f), mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
//			triangle.v3 = {Matrix4x4::multiply(transformation, mesh.positions[mesh.indices[i]], 1.0f), mesh.normals[mesh.indices[i]], mesh.texcoords[mesh.indices[i++]]};
//			triangle.v1 = {Matrix4x4::multiply(transformation, mesh.positions[mesh.indices[i]], 1.0f), Matrix4x4::multiply(transposeInverse, mesh.normals[mesh.indices[i]], 0.0f), mesh.texcoords[mesh.indices[i++]]};
//			triangle.v2 = {Matrix4x4::multiply(transformation, mesh.positions[mesh.indices[i]], 1.0f), Matrix4x4::multiply(transposeInverse, mesh.normals[mesh.indices[i]], 0.0f), mesh.texcoords[mesh.indices[i++]]};
//			triangle.v3 = {Matrix4x4::multiply(transformation, mesh.positions[mesh.indices[i]], 1.0f), Matrix4x4::multiply(transposeInverse, mesh.normals[mesh.indices[i]], 0.0f), mesh.texcoords[mesh.indices[i++]]};
			triangles->push_back(triangle);
		}
	}
	return triangles;
}

void dropCallback(GLFWwindow* window, int count, const char** paths) {
	string path = string(paths[0]);
	replace(path.begin(), path.end(), '\\', '/');
	cout << path << endl;

	removeCurrentModel();
	loadModel(path);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void saveBuffer() {
	GLubyte* pixels = new GLubyte[4 * window->width() * window->height()] { 0 };
	glReadPixels(0, 0, window->width(), window->height(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	for (int i = 0; i < (window->height() / 2); i++) {
		std::swap_ranges(pixels + window->width() * (i) * 4, pixels + (window->width() * (i + 1) * 4), pixels + ((window->height() - 1 - i) * window->width()) * 4);
	}

	unsigned error = lodepng::encode(string(config.output), pixels, window->width(), window->height());
	cout << error << " " << config.output << endl;
}

void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		removeCurrentModel();
//	} else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
//		camera.position = camera.position + camera.getFront();
//	} else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
//		Vector3 side = Vector3::cross(camera.getFront(), Vector3(0.0f, 1.0f, 0.0f));
//		side.normalize().negate();
//		camera.position = camera.position + side;
//	} else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
//		camera.position = camera.position - camera.getFront();
//	} else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
//		Vector3 side = Vector3::cross(camera.getFront(), Vector3(0.0f, 1.0f, 0.0f));
//		side.normalize();
//		camera.position = camera.position + side;
//	} else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
//	} else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
//		renderer->toggleMsaa();
	} else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		renderer->toggleAmbientLight();
	} else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		renderer->toggleSpecularLight();
	} else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		renderer->toggleDiffuseLight();
	} else if (key == GLFW_KEY_T && action == GLFW_PRESS) {

		raytrace(m);
	} else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		saveBuffer();
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

void cursorCallback(GLFWwindow* w, double xpos, double ypos) {
	static double oldX = window->cursorX(), oldY = window->cursorY();
	if (window->isButtonPressed(GLFW_KEY_SPACE)) {
		camera.yaw += (oldX - xpos) / 250.0;
		camera.pitch -= (oldY - ypos) / 250.0;
	}
//	cout << oldX << " " << oldY << "    " << xpos << " " << ypos << endl;
	oldX = xpos;
	oldY = ypos;
}

Matrix4x4 transformations() {
	Matrix4x4 model = Matrix4x4::identityMatrix();
//	Matrix4x4 translate = Matrix4x4::translationMatrix(Vector3(-(m->bounding.getMax().x + m->bounding.getMin().x) / 2.0f, -(m->bounding.getMax().y + m->bounding.getMin().z) / 2.0f, -(m->bounding.getMax().y + m->bounding.getMin().z) / 2.0f - 3.0f));

//	return model * translate;
	return model;
}

class RaytraceStatistics {
	public:
		std::atomic<int> tests;
		std::atomic<int> intersections;
		std::atomic<int> rays;
		RaytraceStatistics() noexcept :tests(0), intersections(0), rays(0) {};
	};

std::atomic<int> pixelCounter(0);
//int pixelCounter = 0;

void rt(Model* model, KdTree* tree, unsigned char* data, int h, int n, RaytraceStatistics* stats) {
	const int w = window->width();
	const int totalPixels = w * window->height();

//	for (int y = h; y < n; y++) {
//		for (int x = 0; x < w; x++) {
	while (pixelCounter < totalPixels) {
		int currentPixel = pixelCounter++;
		int x = currentPixel % w;
		int y = currentPixel / w;
		Vector3 color { };
		const int N = 1500;
		for (int i = 0; i < N; i++) {
			Ray ray = Ray::createRay(camera, x, y, w, window->height(), window->aspectRatio());
			Vector3 result = shotRay(ray, nullptr, 1);
			color += (1.0 / N) * Vector3 { clamp(result.x), clamp(result.y), clamp(result.z) };
		}

		data[(x + y * w) * 3 + 0] = color.x * 255;
		data[(x + y * w) * 3 + 1] = color.y * 255;
		data[(x + y * w) * 3 + 2] = color.z * 255;
//		}
		if ((pixelCounter % 100) == 0) {
			cout << pixelCounter << "/" << (window->height() * w) << endl;
		}
	}
}

void raytrace(Model* model) {
	pixelCounter = 0;
	clock_t begin = clock();

	unsigned char data[window->width() * window->height() * 3] = { 0 };
	const int q = std::thread::hardware_concurrency();
	vector<thread> threads;
	int h = window->height();

	RaytraceStatistics* stats = new RaytraceStatistics();
	stats->tests = 0;
	stats->intersections = 0;

	for (int i = 0; i < q; i++) {
		int from = h / q * i;
		int to = from + h / q;
//		cout << from << " " << to << endl;
		threads.push_back(thread(rt, model, kdTree, &data[0], from, to, stats));
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

void loadModel(string path) {
	m = new Model(path, true);
	om = new OpenglModel(*m);
	modelTriangles = modelToTriangles(m, transformations());
	kdTree = new KdTree(*modelTriangles, X);

	printf("depth %u\n", depth(kdTree));
	printf("triangles %u\n", countTriangles(kdTree));
	printf("leafs %u\n", countLeafs(kdTree));
	printf("subtrees %u\n", countSubtrees(kdTree));
	printf("average %lf\n", averageTrianglesPerLeaf(kdTree));
}

//float distribution(float x) {
//	return pow(sin(x), 3.0) / 10.0;
//}

class Line {
	public:
		Vector3 v1 { }, v2 { };
		Vector4 color { 1.0f, 1.0f, 1.0f, 1.0f };
		float progress = 0.0f;
};

class Light {
	public:
		Vector3 position { };
		Vector3 color { };
};

Light light1 = { Vector3 { 2.0f, 2.0f, 0.0f }, Vector3 { .4f, .4f, .4f } };

vector<Line> lines;
set<const KdTree*> trees { };
set<const Triangle*> interTriangles { };
const KdTree* leaf = nullptr;

std::random_device rd;
std::mt19937 gen(rd());
//std::uniform_real_distribution<float> fiDis(-M_PI / 2.0, M_PI / 2.0);
//std::uniform_real_distribution<float> phiDis(-M_PI / 4.0f, M_PI / 4.0f);
std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

std::mt19937 genRussianRoulette(rd());
std::uniform_real_distribution<float> russianRouletteDistribution(0.0f, 1.0f);

Vector3 albedo { .1f, .3f, .3f };

//http://corysimon.github.io/articles/uniformdistn-on-sphere/
Vector3 randomVectorOnHemisphere(const Vector3& normal) {
	float theta = 2 * M_PI * distribution(gen);
	float phi = acos(1 - 2 * distribution(gen));
	float x = sin(phi) * cos(theta);
	float y = sin(phi) * sin(theta);
	float z = cos(phi);
	Vector3 dir { x, y, z };
	if (Vector3::dot(normal, dir) < 0.0f) {
		dir.negate();
	}
	return dir.normalize();
}

Vector3 shotRay(const Ray& ray, const Triangle* origin, int n) {
	if (n > 11 || (n > 5 && russianRouletteDistribution(genRussianRoulette) > 0.7f)) {
//		return Vector3 { 0.75f, 0.7f, 0.7f }; //background
		return Vector3 { };
	}

	unique_ptr<KdTreeTraversalResult> traverseResult(new KdTreeTraversalResult);

	find(kdTree, ray, origin, traverseResult.get());
	Vector3 result { };
	Vector3 directLight { };
	Vector3 indirectLight { };

	if (!traverseResult->didHit()) {
		return Vector3 { 0.75f, 0.7f, 0.7f }; //background
//		return Vector3 {};
	} else {
		const Vector3 intersectionPoint = *traverseResult->intersectionPoint;
//		const Vector3 normal = traverseResult->triangle->normal();
		const Vector3 normal = traverseResult->triangle->v1.normal;

		{
			Vector3 lightDir = (light1.position - intersectionPoint).normalize();
			Ray lightRay { intersectionPoint, lightDir };
			if (!findAny(kdTree, lightRay, traverseResult->triangle)) {
				directLight = max(0.0f, Vector3::cosineAngle(normal, lightDir)) * light1.color;
//				directLight = Vector3::cosineAngle(normal, lightDir) * light1.color;
//				directLight = light1.color;
			}
		}

//		SphericalVector sv { normal };
//		sv.fi += fiDis(gen);
//		sv.phi += phiDis(gen);
//		Vector3 randomHemisphereDir = Vector3 { sv }.normalize();
		Vector3 randomHemisphereDir = randomVectorOnHemisphere(normal);

		Ray newRay { intersectionPoint, randomHemisphereDir };
//		Vector3 idealReflectionDir = (ray.getDirection() - (2 * Vector3::dot(ray.getDirection(), normal) * normal)).normalize();

		float pdf = 1.0 / (2.0f * M_PI);
		float theta = max(0.0f, Vector3::cosineAngle(normal, randomHemisphereDir));
		indirectLight = theta * shotRay(newRay, traverseResult->triangle, n + 1) / pdf;
		result = (directLight + 2 * indirectLight) / M_PI * albedo;
//		float pdf = 1.0 / (2.0f * M_PI);
//		float theta = Vector3::cosineAngle(normal, randomHemisphereDir);
//		indirectLight = theta * shotRay(newRay, traverseResult->triangle, n + 1) / pdf;
//		result = (directLight / M_PI + indirectLight) * albedo;

//		indirectLight = albedo * shotRay(newRay, traverseResult->triangle, n + 1);
//		result = directLight + indirectLight;
//		shotRay(Ray { intersectionPoint, randomHemisphereDir }, traverseResult->triangle, n + 1)
	}
//	if(n == 1) {
//		cout << "indirect: " << indirectLight << ", direct: " << directLight << ", result: " << result << endl;
//	}
	return result;
}

/*
 Vector3 shotRay(const Ray& ray, const Triangle* origin, int n) {
 if (n > 11 || (n > 2 && russianRouletteDistribution(genRussianRoulette) > 0.7f)) {
 return Vector3 { };
 }

 KdTreeTraversalResult* traverseResult = new KdTreeTraversalResult;

 find(kdTree, ray, origin, traverseResult);
 Vector3 light { };

 if (!traverseResult->didHit()) {
 cout << "zero" << endl;
 lines.push_back(Line { ray.getOrigin(), ray.getOrigin() + 50 * ray.getDirection(), Vector4 { 0.0f, 0.0f, 0.0f, 1.0f } });
 return light;
 } else {
 const Vector3 intersectionPoint = *traverseResult->intersectionPoint;
 interTriangles.insert(traverseResult->intersectedTriangles.begin(), traverseResult->intersectedTriangles.end());
 trees.insert(traverseResult->traversedLeafs.begin(), traverseResult->traversedLeafs.end());
 lines.push_back(Line { ray.getOrigin(), intersectionPoint, Vector4 { 0.0f, 0.0f, 0.0f, 1.0f } });

 const Vector3 normal = traverseResult->triangle->normal();
 Vector3 idealReflectionDir = (ray.getDirection() - (2 * Vector3::dot(ray.getDirection(), normal) * normal)).normalize();

 SphericalVector sv { normal };
 //		float fi = distribution(dis(gen));
 //		float phi = distribution(dis(gen));
 //		cout << fi << ", " << phi << ", sv: " << sv << ", nsv:";
 sv.fi += fiDis(gen);
 sv.phi += phiDis(gen);
 Vector3 randomHemisphereDir = Vector3 { sv }.normalize();
 //		cout << sv << ", sdir: " << secondaryDir << endl;

 Vector3 lightDir = (light1.position - intersectionPoint).normalize();
 {
 lines.push_back(Line { intersectionPoint + 50 * lightDir, intersectionPoint, Vector4 { 1.0f, 1.0f, 0.0f, 1.0f } });
 Ray lightRay { intersectionPoint, lightDir };
 if (!findAny(kdTree, lightRay, traverseResult->triangle)) {
 light = light + light1.color;
 }
 }

 //		cout << "triangle: " << traverseResult->triangle->id << ", inters " << *traverseResult->intersectionPoint << " raydir: " << ray.getDirection() << ", newdir" << newDir << ", normal " << normal << endl;
 leaf = traverseResult->leaf;
 light = light + Vector3::cosineAngle(lightDir, ) * shotRay(Ray { intersectionPoint, randomHemisphereDir }, traverseResult->triangle, n + 1);
 }
 delete traverseResult;
 return (1.0 / n) * light;
 }
 */
void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Ray ray = Ray::createRay(camera, window->cursorX(), window->cursorY(), window->width(), window->height(), window->aspectRatio());

		lines.clear();
		trees.clear();
		interTriangles.clear();
		leaf = nullptr;
//		cout << shotRay(ray, nullptr, 1) << endl;
	}
}

int main(int argc, char** argv) {
	if (!loadConfig(argv[1], config)) {
		cout << "unable to load config file\n" << endl;
	}
//	if(strcmp(argv[2], "batch") == 0) {
//		gui = false;
//	}

	config.print(cout);

	camera.setPosition(config.viewPoint);
	camera.lookAt(config.lookAt);
	camera.setUp(config.up);

	window = Window::createWindow(config.xres, config.yres);
	window->setFramebufferSizeCallback(framebufferSizeCallback);
//	window->setScrollCallback(scrollCallback);
	window->setDropCallback(dropCallback);
	window->setKeyCallback(keyCallback);
	window->setMouseButtonCallback(mouseButtonCallback);
	window->setWindowSizeCallback(windowSizeCallback);
	window->setCursorCallback(cursorCallback);

	renderer = new Renderer(window.get(), "shaders/vertex.glsl", "shaders/fragment.glsl");
	updateWindowTitle();

	loadModel(string(config.input));

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

			if (window->isButtonPressed(GLFW_KEY_W)) {
				camera.position = camera.position + (1.0f / 20.0f * camera.getFront());
			}
			if (window->isButtonPressed(GLFW_KEY_A)) {
				Vector3 side = Vector3::cross(camera.getFront(), Vector3(0.0f, 1.0f, 0.0f));
				side.normalize().negate();
				camera.position = camera.position + (1.0f / 20.0f * side);
			}
			if (window->isButtonPressed(GLFW_KEY_S)) {
				camera.position = camera.position - (1.0f / 20.0f * camera.getFront());
			}
			if (window->isButtonPressed(GLFW_KEY_D)) {
				Vector3 side = Vector3::cross(camera.getFront(), Vector3(0.0f, 1.0f, 0.0f));
				side.normalize();
				camera.position = camera.position + (1.0f / 20.0f * side);
			}
			if (m) {
				renderer->draw(*om, camera, transformations());
//				renderer->drawKdTree(kdTree, camera);
//				renderer->drawLine(Matrix4x4::multiply(transformations(), m->bounding.getMin(), 1.0f), Matrix4x4::multiply(transformations(), m->bounding.getMax(), 1.0f), camera, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

//				glLineWidth(0.5);
//				for (Triangle triangle : *modelTriangles) {
//
//					Vector3 middle = triangle.middle();
//					renderer->drawLine(middle, middle + 0.2 * triangle.v1.normal, camera, Vector4 { 0.0f, 0.0f, 0.8f, 1.0f });
//				}
//				glLineWidth(3);

				for (Line& line : lines) {
					line.progress = 1.0f;
//					line.progress += 0.05f;
					if (line.progress > 1.0f) line.progress = 1.0f;
					renderer->drawLine(line.v1, line.v1 + (line.progress * (line.v2 - line.v1)), camera, line.color);
					if (line.progress < 1.0f) {
						break;
					}
				}
				renderer->drawPoint(light1.position, camera, Vector4 { 1.0f, 1.0f, 0.0f, 1.0f });
				renderer->drawPoint(Vector3 { }, camera, Vector4 { 0.0f, 0.0f, 0.0f, 1.0f });
//				for (const KdTree* tree : trees) {
//					renderer->drawAABB(tree->getAABB(), camera, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
//					renderer->drawKdTree(tree, camera);
//				}
//				for (const Triangle* triangle : interTriangles) {
//					renderer->drawAABB(tree->aabb, camera, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
//					renderer->drawTriangle(*triangle, camera, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
//				}
//				renderer->setLineWidth(4.0f);
//				if (leaf) {
//					if (leaf->getAxis() == X) {
//						renderer->drawAABB(leaf->getAABB(), camera, Vector4 { 1.0f, 0.0f, 0.0f, 1.0f });
//					} else if (leaf->getAxis() == Y) {
//						renderer->drawAABB(leaf->getAABB(), camera, Vector4 { 0.0f, 1.0f, 0.0f, 1.0f });
//					} else {
//						renderer->drawAABB(leaf->getAABB(), camera, Vector4 { 0.0f, 0.0f, 1.0f, 1.0f });
//					}
//					renderer->drawKdTree(leaf, camera);
//					renderer->drawAABB(leaf->getAABB(), camera, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
//				}
//				renderer->setLineWidth(1.0f);
			}

			window->swapBuffers();
		} else {
			this_thread::sleep_for(chrono::milliseconds(2));
			updateWindowTitle();
		}
		window->pollEvents();
//		window->setCursorPosition(window->width() / 2.0, window->height() / 2.0);
	}

	window->terminate();
	return 0;
}

