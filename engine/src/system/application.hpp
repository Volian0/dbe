#pragma once

#include <memory>
#include <string>

#include "int.hpp"
#include "graphics/window.hpp"

class Application {
protected:
	std::shared_ptr<Window> m_window;
public:
	struct Config {
		u32 width;
		u32 height;
		std::string title;
		bool fullscreen;
	};

	virtual void on_init() = 0;
	virtual void on_update() = 0;
	virtual void on_destroy() = 0;

	virtual ~Application();

	void run(const Config& config);
};
