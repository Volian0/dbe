#include "window.hpp"
#include "int.hpp"
#include "logger.hpp"

#include <stdexcept> //for std::runtime_error

static void APIENTRY gl_debug_callback(u32 source, u32 type, u32 id,
	u32 severity, i32 length, const char* message, const void* up) {

	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
		return;
	}

	const char* s;
	const char* t;
	LogSeverity se;

	switch (source) {
		case GL_DEBUG_SOURCE_API: s = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: s = "window system"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: s = "shader compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: s = "third party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: s = "application"; break;
		case GL_DEBUG_SOURCE_OTHER: s = "other"; break;
	}

	switch (type) {
		case GL_DEBUG_TYPE_ERROR: t = "type error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: t = "deprecated behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: t = "undefined behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY: t = "portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: t = "performance"; break;
		case GL_DEBUG_TYPE_MARKER: t = "marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: t = "push group"; break;
		case GL_DEBUG_TYPE_POP_GROUP: t = "pop group"; break;
		case GL_DEBUG_TYPE_OTHER: t = "other"; break;
	}

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
		case GL_DEBUG_SEVERITY_MEDIUM:
			se = LOG_ERROR; break;
		case GL_DEBUG_SEVERITY_LOW:
			se = LOG_WARNING; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			se = LOG_INFO; break;
	}

	log(se, "OpenGL (source: %s; type: %s): %s", s, t, message);
}

static void window_resize_callback(GLFWwindow* ptr, i32 width, i32 height) {
	Window* window = (Window*)glfwGetWindowUserPointer(ptr);

	glViewport(0, 0, width, height);

	window->width = width;
	window->height = height;
}

Window::Window(unsigned width, unsigned height, const std::string& title, bool fullscreen)
	: width(width), height(height)
{
	//make sure to init GLFW
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to init GLFW!");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	glfwSetWindowUserPointer(ptr, this);

	glfwSetFramebufferSizeCallback(ptr, window_resize_callback);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gl_debug_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
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
	now_time = glfwGetTime();
	timestep = now_time - last_time;
	last_time = now_time;

	glfwSwapBuffers(ptr);
}
