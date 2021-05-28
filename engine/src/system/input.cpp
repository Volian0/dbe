#include "input.hpp"

void InputManager::input_key_callback(GLFWwindow* ptr, int key, int scancode, int action, int mods)
{
	Window* window = (Window*)glfwGetWindowUserPointer(ptr);
	InputManager* input_manager = window->input_manager;
	if (action == GLFW_PRESS) {
		input_manager->_held_keys.emplace(key);
	}
	else if (action == GLFW_RELEASE) {
		input_manager->_held_keys.erase(key);
	}
}

InputManager::InputManager(Window& window)
{
	glfwSetKeyCallback(window.ptr, input_key_callback);
	window.input_manager = this;
}

void InputManager::update()
{
	for (auto it = _key_states.begin(); it != _key_states.end();) {
		int key = it->first;
		auto& state = it->second;
		if (state == KeyState::RELEASED) {
			if (_held_keys.count(key)) {
				state = KeyState::PRESSED;
			}
			else {
				_key_states.erase(it++);
				continue;
			}
		}
		else if (_held_keys.count(key)) {
			state = KeyState::HELD;
		}
		else {
			state = KeyState::RELEASED;
		}
		++it;
	}
	for (int key : _held_keys) {
		if (!_key_states.count(key)) {
			_key_states[key] = KeyState::PRESSED;
		}
	}
}

bool InputManager::is_held(int key) const
{
	return _held_keys.count(key);
}

KeyState InputManager::key_state(int key) const
{
	if (_key_states.count(key))
	{
		return _key_states.at(key);
	}
	return KeyState::UNHELD;
}
