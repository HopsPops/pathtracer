#include "shader.hpp"
#include "mesh.hpp"
#include "model.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "lodepng.h"

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat2x2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <thread>

#include <string>
#include <vector>

#include <iostream>

using namespace std;

const unsigned int DEFAULT_WIDTH = 800;
const unsigned int DEFEULT_HEIGHT = 600;

GLFWwindow* window;
bool stateChanged = true;
float zOffset = 10.0f;
model* m = nullptr;
bool rotationEnabled = true;
bool rotationInversed = false;
float rotation = 0.0f;
bool msaa = true;
bool ambient = true;
bool specular = true;
bool diffuse = true;
string path;

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

int width() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return width;
}

int height() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

void removeCurrentModel() {
	if (m != nullptr) {
		delete m;
		m = nullptr;
	}
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	zOffset += yoffset;
	if (zOffset < 0.0f) zOffset = 0.0f;
	if (zOffset > 100.0f) zOffset = 100.0f;
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

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	stateChanged = true;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		rotationEnabled = !rotationEnabled;
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
	} else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		GLubyte* pixels = new GLubyte[4 * width() * height()] { 0 };
		glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		for (int i = 0; i < (height() / 2); i++) {
			std::swap_ranges(pixels + width() * (i) * 4, pixels + (width() * (i + 1) * 4), pixels + ((height() - 1 - i) * width()) * 4);
		}

		unsigned error = lodepng::encode(path, pixels, width(), height());
		cout << error << " " << path << endl;
	}
}

float aspectRatio() {
	float ar = (float) width() / (float) height();
	if (isnan(ar)) return 1.0f; //przy minimalizowaniu okna height() czasami zwraca 0 co daje nan
	return ar;
}

void updateWindowTitle() {
	string title("modelv [");
	title += rotationEnabled ? "r" : "-";
	title += ambient ? "a" : "-";
	title += specular ? "s" : "-";
	title += diffuse ? "d" : "-";
	title += msaa ? "m" : "-";
	title += "]";
	glfwSetWindowTitle(window, title.c_str());
}

bool shouldUpdate() {
	return rotationEnabled || stateChanged;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "Pass path to where framebuffer has to be saved as first argument" << endl;
		return 1;
	}
	path = string(argv[1]);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(DEFAULT_WIDTH, DEFEULT_HEIGHT, "raytrace", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetDropCallback(window, dropCallback);
	glfwSetKeyCallback(window, keyCallback);
	updateWindowTitle();

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	shader s("vertex.glsl", "fragment.glsl");

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;

		if (deltaTime > 1.0f / 60.0f && shouldUpdate()) {
			lastFrame = currentFrame;

			processInput(window);
			glClearColor(0.75f, 0.7f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (m) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-(m->bounding.max.x + m->bounding.min.x) / 2.0f, -(m->bounding.max.y + m->bounding.min.z) / 2.0f, -(m->bounding.max.y + m->bounding.min.z) / 2.0f - 3.0f - zOffset));
				model = glm::rotate(model, rotation, glm::vec3 { 0.0f, 1.0f, 0.0f });

				if (rotationEnabled) {
					rotation += rotationInversed ? -0.03f : 0.03;
				}

				glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspectRatio(), 0.1f, 100.0f);
				glm::mat4 view = glm::mat4(1.0f);

				s.use();
				s.set("projection", proj);
				s.set("view", view);
				s.set("model", model);

				s.set("ambientColor", glm::vec3 { 1.0f, 1.0f, 1.0f });
				s.set("lightPosition", glm::vec3 { 0.0f, 0.0f, 0.0f });
				s.set("cameraPosition", glm::vec3 { 0.0f, 0.0f, 0.0f });
				s.set("colorOverride", glm::vec4 { 1.0f, 1.0f, 1.0f, 0.0f });

				s.set("ambientLight", ambient);
				s.set("specularLight", specular);
				s.set("diffuseLight", diffuse);

				draw(s, *m);
			}

			glfwSwapBuffers(window);
			stateChanged = false;
		} else {
			this_thread::sleep_for(chrono::milliseconds(2));
			updateWindowTitle();
		}
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

