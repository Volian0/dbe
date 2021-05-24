#include "window.hpp"

#include <stdexcept> //for std::runtime_error

Window::Window(unsigned width, unsigned height, const std::string& title, bool fullscreen)
{
	//make sure to init GLFW
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to init GLFW!");
	}

	//create the window
	ptr = glfwCreateWindow(width, height, title.c_str(), fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	if (!ptr)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create window!");
	}

	glfwMakeContextCurrent(ptr);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		throw std::runtime_error("Failed to init GLAD!");
	}
}

Window::~Window()
{
	//destroy the window
	glfwDestroyWindow(ptr);

	//cleanup
	glfwTerminate();
}

void Window::event_update() const {
	glfwPollEvents();
}

bool Window::should_close() const
{
	return glfwWindowShouldClose(ptr);
}

void Window::swap_buffers()
{
	glfwSwapBuffers(ptr);
}
