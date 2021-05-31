#include <engine.hpp>

class Sandbox : public Application {
private:
	Scene m_scene;

	double m_next_reload { 1.0 };

	Entity monkey;

	std::shared_ptr<InputManager> m_input_manager;
public:
	void on_init() override {
		m_input_manager = std::make_shared<InputManager>(*m_window);

		m_scene.m_renderer->new_shader("postprocess",
			ResourceManager::load_string("shaders/postprocess.glsl"));

		m_scene.m_renderer->new_shader("depth",
			ResourceManager::load_string("shaders/depth.glsl"));

		m_scene.m_renderer->new_shader("cel",
			ResourceManager::load_string("shaders/cel.glsl"));

		m_scene.m_renderer->init("postprocess", "depth");

		monkey = load_model(m_scene, "res/monkey.glb", "cel");
		monkey.get_component<Transform>().translation.z = -3.0;
		monkey.get_component<Transform>().rotation.y = 25.0;

		m_scene.m_light_renderer->m_sun.direction = { 0.5, -1.0, -1.0 };
	}

	void on_update() override {
		//handle input
		m_input_manager->update();

		if (m_input_manager->key_state(GLFW_KEY_ESCAPE) == KeyState::PRESSED)
		{
			return m_window->close();
		}

		m_scene.m_renderer->m_projection = mat4::persp(75.0,
			(float)m_window->width/(float)m_window->height,
			0.1, 100.0);

		m_scene.render(vec2{(float)m_window->width, (float)m_window->height});

		m_next_reload -= m_window->timestep;
		if (m_next_reload <= 0.0) {
			m_next_reload = 1.0;
			ResourceManager::hot_reload();
		}
	}

	void on_destroy() override {

	}
};

int main() {
	Sandbox sbox;
	sbox.run({
		640, 480,
		"Sandbox",
		false
	});
}
