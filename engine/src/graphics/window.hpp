#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Window
{
public:
	//create a window
	Window(unsigned width, unsigned height, const std::string& title, bool fullscreen);

	~Window();

	bool should_close() const;

	void swap_buffers();

private:
	GLFWwindow* ptr;
};

