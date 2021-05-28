#pragma once

#include "../graphics/window.hpp"

#include <unordered_map>
#include <unordered_set>

enum class KeyState : uint8_t
{
	PRESSED, HELD, RELEASED, UNHELD
};

class InputManager
{
public:
	InputManager(Window& window);

	//call this every frame
	void update();

	bool is_held(int key) const;
	KeyState key_state(int key) const;

private:
	static void input_key_callback(GLFWwindow* ptr, int key, int scancode, int action, int mods);
	std::unordered_map<int, KeyState> _key_states;
	std::unordered_set<int> _held_keys;
};

