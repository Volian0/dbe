#pragma once

#include <memory>

#include "graphics/window.hpp"

class GUI {
private:
	static GUI& instance() {
		static GUI i;
		return i;
	}
public:
	static void init(const std::shared_ptr <Window>& window);
	static void quit();
	static void begin_frame();
	static void end_frame();
};
