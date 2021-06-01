#pragma once

#include <memory>

#include "graphics/window.hpp"

struct ImFont;

class GUI {
private:
	static GUI& instance() {
		static GUI i;
		return i;
	}

	ImFont* small_icons;
	ImFont* big_icons;
public:
	static void init(const std::shared_ptr <Window>& window);
	static void quit();
	static void begin_frame();
	static void end_frame();
	static void use_big_icons();
	static void use_small_icons();
};
