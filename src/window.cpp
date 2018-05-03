#include <iostream>
#include <math.h>
#include <window.hpp>
#include <functional>
//#include "glad.c"

using namespace std;

const unsigned int DEFAULT_WIDTH = 800;
const unsigned int DEFEULT_HEIGHT = 600;

unique_ptr<Window> Window::createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	Window* w = new Window();
	w->handle = glfwCreateWindow(DEFAULT_WIDTH, DEFEULT_HEIGHT, "", NULL, NULL);
	if (w->handle == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return unique_ptr<Window>(nullptr);
	}

	glfwMakeContextCurrent(w->handle);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return unique_ptr<Window>(nullptr);
	}
//	glfwDefaultWindowHints();
	return unique_ptr<Window>(w);
}

void Window::setTitle(const char* title) {
	glfwSetWindowTitle(handle, title);
}

int Window::width() {
	int width, height;
	glfwGetWindowSize(handle, &width, &height);
	return width;
}

int Window::height() {
	int width, height;
	glfwGetWindowSize(handle, &width, &height);
	return height;
}

void Window::terminate() {
	glfwTerminate();
}

void Window::swapBuffers() {
	glfwSwapBuffers(handle);
}

void Window::pollEvents() {
	if (glfwGetKey(handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(handle, true);
	}
	glfwPollEvents();
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(handle);
}

double Window::aspectRatio() {
	float ar = (float) width() / (float) height();
	if (isnan(ar)) return 1.0f; //przy minimalizowaniu okna height() czasami zwraca 0 co daje nan
	return ar;
}

void Window::setFramebufferSizeCallback(GLFWframebuffersizefun f) {
	glfwSetFramebufferSizeCallback(handle, f);
}

void Window::setScrollCallback(GLFWscrollfun f) {
	glfwSetScrollCallback(handle, f);
}

void Window::setDropCallback(GLFWdropfun f) {
	glfwSetDropCallback(handle, f);
}

void Window::setKeyCallback(GLFWkeyfun f) {
	glfwSetKeyCallback(handle, f);
}

void Window::setMouseButtonCallback(GLFWmousebuttonfun f) {
	glfwSetMouseButtonCallback(handle, f);
}

void Window::setWindowSizeCallback(GLFWwindowsizefun f) {
	glfwSetWindowSizeCallback(handle, f);
}

double Window::cursorX() {
	double xpos = 0, ypos = 0;
	glfwGetCursorPos(handle, &xpos, &ypos);
	return xpos;
}

double Window::cursorY() {
	double xpos = 0, ypos = 0;
	glfwGetCursorPos(handle, &xpos, &ypos);
	return ypos;
}
