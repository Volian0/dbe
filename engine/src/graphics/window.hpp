#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Window
{
public:
	double timestep { 0.0 };

	//create a window
	Window(unsigned width, unsigned height, const std::string& title, bool fullscreen);

	~Window();

	bool should_close() const;

	void swap_buffers();

	void event_update() const;

private:
	GLFWwindow* ptr;

	double last_time = 0.0, now_time = 0.0;
};

