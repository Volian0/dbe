#pragma once

#include "int.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Window
{
	friend class InputManager;

public:
	double timestep { 0.0 };

	u32 width = 0, height = 0;

	//create a window
	Window(unsigned width, unsigned height, const std::string& title, bool fullscreen);

	~Window();

	bool should_close() const;

	void swap_buffers();

	void event_update() const;

	void close();

private:
	GLFWwindow* ptr;
	class InputManager* input_manager;

	double last_time = 0.0, now_time = 0.0;
};

