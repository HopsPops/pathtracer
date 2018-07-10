#include <3dtree.hpp>
#include <camera.hpp>
#include <config.hpp>
#include <lodepng.h>
#include <math.hpp>
#include <model.hpp>
#include <pathtracer.hpp>
#include <renderer.hpp>
#include <string.h>
#include <triangle.hpp>
#include <GLFW/glfw3.h>
#include <window.hpp>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace std;

void loadModel(string);

unique_ptr<Window> window;
Renderer* renderer;
OpenglModel* om;
Triangles* modelTriangles;
KdTree* kdTree;
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

Triangles lights { };

void dropCallback(GLFWwindow* window, int count, const char** paths) {
	string path = string(paths[0]);
	replace(path.begin(), path.end(), '\\', '/');
	cout << path << endl;

	removeCurrentModel();
	loadModel(path);
	om = new OpenglModel(*m);
}

void loadModel(string path) {
	m = new Model(path);
	modelTriangles = modelToTriangles(m, lights);
	kdTree = new KdTree(*modelTriangles, 8 + 1.3 * log(modelTriangles->size()));

	printf("depth %u\n", depth(kdTree));
	printf("triangles %u\n", countTriangles(kdTree));
	printf("leafs %u\n", countLeafs(kdTree));
	printf("subtrees %u\n", countSubtrees(kdTree));
	printf("average %lf\n", averageTrianglesPerLeaf(kdTree));
}

vector<Line> lines;
vector<Point> points;
vector<const KdTree*> trees { };
vector<const Triangle*> interTriangles { };
const KdTree* leaf = nullptr;

void savePNG(unsigned char* data, int width, int height) {
	unsigned error = lodepng::encode(string(config.output), data, width, height, LCT_RGB);
	cout << error << " " << config.output << endl;
}

void render(const KdTree* tree, const Triangles& triangles, const Camera& camera, const TraceRequest& request) {
	unsigned char data[request.width * request.height * 3] = { 0 };
	pathtrace(kdTree, triangles, camera, data, request);
	savePNG(data, request.width, request.height);
}

int main(int argc, char** argv) {
	if (argc > 1) {
		if (!loadConfig(argv[1], config)) {
			cout << "unable to load config file\n" << endl;
			return 1;
		}
	} else {
		cout << "pass rtc config file as argument" << endl;
		return 1;
	}
	config.print(cout);

	camera.setPosition(config.viewPoint);
	camera.lookAt(config.lookAt);
	camera.setUp(config.up);

	loadModel(string(config.input));

	if (argc > 2 && (strcmp(argv[2], "batch") == 0)) {
		render(kdTree, *modelTriangles, camera, TraceRequest(config));
		return 0;
	}

	window = Window::createWindow(config.xres, config.yres);

	window->setFramebufferSizeCallback([](GLFWwindow* window, int width, int height) {
		renderer->setViewport(width, height);
	});

	window->setDropCallback(dropCallback);
	window->setKeyCallback([](GLFWwindow* win, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_X && action == GLFW_PRESS) {
			removeCurrentModel();
		} else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
			renderer->toggleAmbientLight();
		} else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
			renderer->toggleSpecularLight();
		} else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
			renderer->toggleDiffuseLight();
		} else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
			render(kdTree, *modelTriangles, camera, TraceRequest(config));
		}
	});

	//shooting ray from click location
	window->setMouseButtonCallback([](GLFWwindow* win, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			Ray ray = Ray::createRay(camera, window->cursorX(), window->cursorY(), window->width(), window->height(), window->aspectRatio());
//
			lines.clear();
//			trees.clear();
//			points.clear();
//			interTriangles.clear();
//			leaf = nullptr;
			unique_ptr<TraverseDebug> debug(new TraverseDebug);
			cout << shotRay(kdTree, findLights(*modelTriangles), ray, -1, 0, 0, debug.get()) << endl;
			lines.insert(lines.end(), debug->lines.begin(), debug->lines.end());
		}
	});

	//camera rotations
	window->setCursorCallback([](GLFWwindow* w, double xpos, double ypos) {
		static double oldX = window->cursorX(), oldY = window->cursorY();
		if (window->isButtonPressed(GLFW_KEY_SPACE)) {
			camera.yaw += (oldX - xpos) / 250.0;
			camera.pitch -= (oldY - ypos) / 250.0;
		}
		oldX = xpos;
		oldY = ypos;
	});

	renderer = new Renderer(window.get(), "shaders/vertex.glsl", "shaders/fragment.glsl");
	om = new OpenglModel(*m);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	const float FPS = 30.0f;

	while (!window->shouldClose()) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;

		if (deltaTime > 1.0f / FPS) {
			lastFrame = currentFrame;

			renderer->clearColor(0.75f, 0.7f, 0.7f);
			renderer->clearColorBuffer();
			renderer->clearDepthBuffer();

			{ //camera movement
				float cameraSpeed = 1.0f / 20.0f;
				if (window->isButtonPressed(GLFW_KEY_W)) {
					camera.position += (cameraSpeed * camera.getFront());
				}
				if (window->isButtonPressed(GLFW_KEY_A)) {
					camera.position += (cameraSpeed * camera.getLeft());
				}
				if (window->isButtonPressed(GLFW_KEY_S)) {
					camera.position += (cameraSpeed * camera.getBack());
				}
				if (window->isButtonPressed(GLFW_KEY_D)) {
					camera.position += (cameraSpeed * camera.getRight());
				}
			}

			if (m) {
				renderer->draw(*om, camera);

				renderer->drawAABB(kdTree->getAABB(), camera, Vector4 { 1.0f, 0.7f, 0.6f, 1.0f }, 2.0f); //draw root of kdtree

//				for (const Triangle* triangle : *modelTriangles) { //draw normals
//					Vector3 middle = triangle->middle();
//					renderer->drawLine(middle, middle + 0.1 * triangle->normal(), camera, Vector4 { 0.0f, 0.0f, 0.7f, 1.0f });
//				}

				for (Point& point : points) {
					renderer->drawPoint(point.position, camera, point.color);
				}

				for (Line& line : lines) {
					renderer->drawLine(line.v1, line.v2, camera, line.color);
				}

				renderer->drawPoint(Vector3 { }, camera, Vector4 { 0.0f, 0.0f, 0.0f, 1.0f });
				for (const KdTree* tree : trees) {
					renderer->drawAABB(tree->getAABB(), camera, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
				}

				for (const Triangle* triangle : interTriangles) {
					renderer->drawTriangle(*triangle, camera, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
				}

				if (leaf) {
					if (leaf->getAxis() == 0) {
						renderer->drawAABB(leaf->getAABB(), camera, Vector4 { 1.0f, 0.0f, 0.0f, 1.0f }, 3.0f);
					} else if (leaf->getAxis() == 1) {
						renderer->drawAABB(leaf->getAABB(), camera, Vector4 { 0.0f, 1.0f, 0.0f, 1.0f }, 3.0f);
					} else {
						renderer->drawAABB(leaf->getAABB(), camera, Vector4 { 0.0f, 0.0f, 1.0f, 1.0f }, 3.0f);
					}
				}
			}

			window->swapBuffers();
		} else {
			this_thread::sleep_for(chrono::milliseconds(2));
		}
		window->pollEvents();
	}

	window->terminate();
	return 0;
}

