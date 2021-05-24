#include "engine.hpp"

#include "graphics/window.hpp"

#include <stdio.h>

void print_hello() {
	printf("Hello, world\n");
}

void test_window_loop()
{
	Window window(640, 480, "Test Window", false);
	while (!window.should_close())
	{
		glfwPollEvents();
		window.swap_buffers();
	}
}
