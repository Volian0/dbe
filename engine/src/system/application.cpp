#include <glad/glad.h>

#include "application.hpp"
#include "resources.hpp"

Application::~Application() {}

void Application::run(const Config& config) {
	m_window = std::make_shared<Window>(
		config.width,
		config.height,
		config.title,
		config.fullscreen);

	ResourceManager::init("res");

	on_init();

	while (!m_window->should_close()) {
		m_window->event_update();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		on_update();

		m_window->swap_buffers();
	}

	on_destroy();

	ResourceManager::quit();
}
