#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>
#include <memory>

class Window {
	private:
		GLFWwindow* handle = nullptr;
		public:
		static std::unique_ptr<Window> createWindow(int, int);
		void setTitle(const char*);
		int width();
		int height();

		void terminate();
		void swapBuffers();
		void pollEvents();
		bool shouldClose();

		bool isMouseLeftButtonPressed();
		bool isButtonPressed(int);

		double aspectRatio();

		double cursorX();
		double cursorY();
		void setCursorPosition(double x, double y);

		void setFramebufferSizeCallback(GLFWframebuffersizefun);
		void setScrollCallback(GLFWscrollfun);
		void setDropCallback(GLFWdropfun);
		void setKeyCallback(GLFWkeyfun);
		void setMouseButtonCallback(GLFWmousebuttonfun);
		void setWindowSizeCallback(GLFWwindowsizefun);
		void setCursorCallback(GLFWcursorposfun);

};

