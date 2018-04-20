#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat2x2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

#include "shader.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "ray.hpp"
#include "intersection.hpp"

#include "lodepng.h"

#include <chrono>
#include <thread>

#include <string>
#include <vector>
#include <string.h>

#include <iostream>

#include "types.hpp"

#include "window.hpp"
#include "config.hpp"

using namespace std;

void raytrace();

window win;
bool stateChanged = true;
float zOffset = 10.0f;
model* m = nullptr;
bool rotationEnabled = false;
bool rotationInversed = false;
float rotation = 0.0f;
bool msaa = true;
bool ambient = true;
bool specular = true;
bool diffuse = true;
Config config;

void draw(shader shader, mesh m) {

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	if (m.textures.empty()) {
		shader.set("textures", false);
	} else {
		shader.set("textures", true);
		for (unsigned int i = 0; i < m.textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			string number;
			string name = m.textures[i].type;
			if (name == "texture_diffuse") {
				number = std::to_string(diffuseNr++);
			} else if (name == "texture_specular") {
				number = std::to_string(specularNr++);
			} else if (name == "texture_normal") {
				number = std::to_string(normalNr++);
			} else if (name == "texture_height") {
				number = std::to_string(heightNr++);
			}

			glUniform1i(glGetUniformLocation(shader.getId(), (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, m.textures[i].id);
		}
	}
	glBindVertexArray(m.vao);
	glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void draw(shader shader, model m) {
	for (unsigned int i = 0; i < m.meshes.size(); i++) {
		draw(shader, m.meshes[i]);
	}
}

void removeCurrentModel() {
	if (m != nullptr) {
		delete m;
		m = nullptr;
	}
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	zOffset += yoffset;
//	if (zOffset < 0.0f) zOffset = 0.0f;
//	if (zOffset > 100.0f) zOffset = 100.0f;
//	cout << zOffset << endl;
	stateChanged = true;
}

void dropCallback(GLFWwindow* window, int count, const char** paths) {
	string path = string(paths[0]);
	replace(path.begin(), path.end(), '\\', '/');
	cout << path << endl;

	removeCurrentModel();

	m = new model(path, true);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	stateChanged = true;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
//		rotationEnabled = !rotationEnabled;
	} else if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		removeCurrentModel();
		stateChanged = true;
	} else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		if (msaa) {
			glDisable(GL_MULTISAMPLE);
		} else {
			glEnable(GL_MULTISAMPLE);
		}
		msaa = !msaa;
		stateChanged = true;
	} else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		ambient = !ambient;
		stateChanged = true;
	} else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		specular = !specular;
		stateChanged = true;
	} else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		diffuse = !diffuse;
		stateChanged = true;
	} else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		rotation = 0.0f;
		stateChanged = true;
	} else if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		rotationInversed = !rotationInversed;
		stateChanged = true;
	} else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		raytrace();
	} else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		GLubyte* pixels = new GLubyte[4 * win.width() * win.height()] { 0 };
		glReadPixels(0, 0, win.width(), win.height(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		for (int i = 0; i < (win.height() / 2); i++) {
			std::swap_ranges(pixels + win.width() * (i) * 4, pixels + (win.width() * (i + 1) * 4), pixels + ((win.height() - 1 - i) * win.width()) * 4);
		}

		unsigned error = lodepng::encode(string(config.output), pixels, win.width(), win.height());
		cout << error << " " << config.output << endl;
	}
}

void updateWindowTitle() {
	string title("modelv [");
	title += rotationEnabled ? "r" : "-";
	title += ambient ? "a" : "-";
	title += specular ? "s" : "-";
	title += diffuse ? "d" : "-";
	title += msaa ? "m" : "-";
	title += "]";
	title += " w:h=";
	title += std::to_string(win.width());
	title += ":";
	title += std::to_string(win.height());
//	glfwSetWindowTitle(window, title.c_str());
	win.setTitle(title.c_str());
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
	updateWindowTitle();
}

bool shouldUpdate() {
	return rotationEnabled || stateChanged;
}

camera cam;

mat4 modelMat() {
	mat4 model = mat4(1.0f);
	model = glm::translate(model, vec3(-(m->bounding.max.x + m->bounding.min.x) / 2.0f, -(m->bounding.max.y + m->bounding.min.z) / 2.0f, -(m->bounding.max.y + m->bounding.min.z) / 2.0f - 3.0f - zOffset));
	model = glm::rotate(model, rotation, vec3 { 0.0f, 1.0f, 0.0f });
	return model;
}

vec3 multiply(mat4 mat, vec3 v) {
	return vec3(mat * vec4(v, 1.0f));
}

void rt(unsigned char* data, int h, int n) {
	const int w = win.width();

	mat4 modelMatrix = modelMat();

	for (int y = h; y < n; y++) {
		for (int x = 0; x < w; x++) {
			ray ray = ray::createRay(cam, x, y, w, win.height(), win.aspectRatio());

			if (!intersectAABB(ray, multiply(modelMatrix, m->bounding.min), multiply(modelMatrix, m->bounding.max))) {
				continue;
			}

			float depth = INFINITY;

			for (mesh &mesh : m->meshes) {
				if (!intersectAABB(ray, multiply(modelMatrix, mesh.bounding.min), multiply(modelMatrix, mesh.bounding.max))) {
					continue;
				}

				vec3 intersection(0.0, 0.0, 0.0);
				for (unsigned int i = 0; i < mesh.vertices.size();) {
					vec3 v1 = multiply(modelMatrix, mesh.vertices[i++].position);
					vec3 v2 = multiply(modelMatrix, mesh.vertices[i++].position);
					vec3 v3 = multiply(modelMatrix, mesh.vertices[i++].position);

					if (!intersectTriangle(ray, v1, v2, v3, intersection)) {
						continue;
					}

//					double dist = glm::distance(ray.getOrigin(), intersection);
//
//					if (dist >= depth) {
//						continue;
//					}
//					depth = dist;
					depth = 1.0; //póki co i tak sprawdzamy tylko czy natrafia na obiekt wiêc nie musimy robic depth-testu
					goto escape;
					// wychodzimy z podwójnej pêtli
				}
			}
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

void raytrace() {
	clock_t begin = clock();

	unsigned char data[win.width() * win.height() * 3] = { 0 };
	const int q = std::thread::hardware_concurrency();
	vector<thread> threads;
	int h = win.height();
	for (int i = 0; i < q; i++) {
		int from = h / q * i;
		int to = from + h / q;
		cout << from << " " << to << endl;
		threads.push_back(thread(rt, &data[0], from, to));
	}

	for (auto& th : threads) {
		th.join();
	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << "time " << elapsed_secs << endl;

	unsigned error = lodepng::encode(string(config.output), data, win.width(), win.height(), LCT_RGB);
	cout << error << " " << config.output << endl;
}

bool hit = false;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		ray r = ray::createRay(cam, win.cursorX(), win.cursorY(), win.width(), win.height(), win.aspectRatio());
		r.print(cout);
		mat4 mvp = cam.view() * modelMat();

		vec3 v1 = multiply(mvp, m->meshes[0].vertices[0].position);
		vec3 v2 = multiply(mvp, m->meshes[0].vertices[1].position);
		vec3 v3 = multiply(mvp, m->meshes[0].vertices[2].position);

		cout << "{{" << v1.x << ", " << v1.y << ", " << v1.z << "}, {" << v2.x << ", " << v2.y << ", " << v2.z << "}, {" << v3.x << ", " << v3.y << ", " << v3.z << "}}\n";
		cout << std::flush;
		vec3 result();
	}
}

bool loadConfig(Config& c) {
	FILE* f = fopen("plik.rtc", "r");
	if (!f) {
		return false;
	}

	char buffor[1000] = { 0 };
	while (fgets(buffor, 1000, f) && !feof(f)) {
		sscanf(buffor, "input %500s", c.input);
		sscanf(buffor, "output %500s", c.output);
		sscanf(buffor, "k %u", &c.k);
		sscanf(buffor, "xres %u", &c.xres);
		sscanf(buffor, "yres %u", &c.yres);
		sscanf(buffor, "VPx %f", &c.viewPoint.x);
		sscanf(buffor, "VPy %f", &c.viewPoint.y);
		sscanf(buffor, "VPz %f", &c.viewPoint.z);
		sscanf(buffor, "LAx %f", &c.lookAt.x);
		sscanf(buffor, "LAy %f", &c.lookAt.y);
		sscanf(buffor, "LAz %f", &c.lookAt.z);
		sscanf(buffor, "yview %f", &c.yview);
	}

	replace(&c.input[0], &c.input[500], '\\', '/');
	replace(&c.output[0], &c.output[500], '\\', '/');

	fclose(f);
	return true;
}

int main(int argc, char** argv) {
	if (!loadConfig(config)) {
		cout << "nie udalo sie wczytac pliku konfiguracyjnego" << endl;
	}
	config.print(cout);

//	cam.setPosition(config.viewPoint);
//	cam.lookAt(config.lookAt);
//	cam.setUp(config.up);

	win = *window::createWindow();
	win.setFramebufferSizeCallback(framebufferSizeCallback);
	win.setScrollCallback(scrollCallback);
	win.setDropCallback(dropCallback);
	win.setKeyCallback(keyCallback);
	win.setMouseButtonCallback(mouseButtonCallback);
	win.setWindowSizeCallback(windowSizeCallback);
	updateWindowTitle();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	m = new model(string(config.input), true);

	shader s("shaders/vertex.glsl", "shaders/fragment.glsl");

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!win.shouldClose()) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;

		if (deltaTime > 1.0f / 20.0f && shouldUpdate()) {
			lastFrame = currentFrame;

			glClearColor(0.75f, 0.7f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (m) {
				if (rotationEnabled) {
					rotation += rotationInversed ? -0.03f : 0.03;
				}

				mat4 md = modelMat();
				mat4 proj = cam.perspective(win.aspectRatio());
				mat4 view = cam.view();

				s.use();
				s.set("projection", proj);
				s.set("view", view);
				s.set("model", md);

				s.set("ambientColor", vec3 { 1.0f, 1.0f, 1.0f });
				s.set("lightPosition", vec3 { 0.0f, 0.0f, 0.0f });
				s.set("cameraPosition", vec3 { 0.0f, 0.0f, 0.0f });

//				if (hit) {
//					s.set("colorOverride", vec4 { 1.0f, 0.0f, 0.0f, 0.0f });
//				} else {
				s.set("colorOverride", vec4 { 1.0f, 1.0f, 1.0f, 0.0f });
//				}

				s.set("ambientLight", ambient);
				s.set("specularLight", specular);
				s.set("diffuseLight", diffuse);

				draw(s, *m);
			}

			win.swapBuffers();
			stateChanged = true;
		} else {
			this_thread::sleep_for(chrono::milliseconds(2));
			updateWindowTitle();
		}
		win.pollEvents();
	}

	win.terminate();
	return 0;
}

