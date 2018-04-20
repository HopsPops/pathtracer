#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

class window {
	private:
		GLFWwindow* handle = nullptr;
		public:
		static std::unique_ptr<window> createWindow();
		void setTitle(const char*);
		int width();
		int height();

		void terminate();
		void swapBuffers();
		void pollEvents();
		bool shouldClose();

		double aspectRatio();

		double cursorX();
		double cursorY();

		void setFramebufferSizeCallback(GLFWframebuffersizefun);
		void setScrollCallback(GLFWscrollfun);
		void setDropCallback(GLFWdropfun);
		void setKeyCallback(GLFWkeyfun);
		void setMouseButtonCallback(GLFWmousebuttonfun);
		void setWindowSizeCallback(GLFWwindowsizefun);

};

