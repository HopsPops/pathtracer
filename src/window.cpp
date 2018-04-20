#include "window.hpp"
#include <iostream>
#include <math.h>
#include <functional>

using namespace std;

const unsigned int DEFAULT_WIDTH = 800;
const unsigned int DEFEULT_HEIGHT = 600;

unique_ptr<window> window::createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window* w = new window();
	w->handle = glfwCreateWindow(DEFAULT_WIDTH, DEFEULT_HEIGHT, "", NULL, NULL);
	if (w->handle == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return unique_ptr<window>(nullptr);
	}

	glfwMakeContextCurrent(w->handle);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return unique_ptr<window>(nullptr);
	}
//	glfwDefaultWindowHints();
	return unique_ptr<window>(w);
}

void window::setTitle(const char* title) {
	glfwSetWindowTitle(handle, title);
}

int window::width() {
	int width, height;
	glfwGetWindowSize(handle, &width, &height);
	return width;
}

int window::height() {
	int width, height;
	glfwGetWindowSize(handle, &width, &height);
	return height;
}

void window::terminate() {
	glfwTerminate();
}

void window::swapBuffers() {
	glfwSwapBuffers(handle);
}

void window::pollEvents() {
	if (glfwGetKey(handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(handle, true);
	}
	glfwPollEvents();
}

bool window::shouldClose() {
	return glfwWindowShouldClose(handle);
}

double window::aspectRatio() {
	float ar = (float) width() / (float) height();
	if (isnan(ar)) return 1.0f; //przy minimalizowaniu okna height() czasami zwraca 0 co daje nan
	return ar;
}

void window::setFramebufferSizeCallback(GLFWframebuffersizefun f) {
	glfwSetFramebufferSizeCallback(handle, f);
}

void window::setScrollCallback(GLFWscrollfun f) {
	glfwSetScrollCallback(handle, f);
}

void window::setDropCallback(GLFWdropfun f) {
	glfwSetDropCallback(handle, f);
}

void window::setKeyCallback(GLFWkeyfun f) {
	glfwSetKeyCallback(handle, f);
}

void window::setMouseButtonCallback(GLFWmousebuttonfun f) {
	glfwSetMouseButtonCallback(handle, f);
}

void window::setWindowSizeCallback(GLFWwindowsizefun f) {
	glfwSetWindowSizeCallback(handle, f);
}

double window::cursorX() {
	double xpos = 0, ypos = 0;
	glfwGetCursorPos(handle, &xpos, &ypos);
	return xpos;
}

double window::cursorY() {
	double xpos = 0, ypos = 0;
	glfwGetCursorPos(handle, &xpos, &ypos);
	return ypos;
}
