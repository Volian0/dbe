#include <engine.hpp>

class Sandbox : public Application {
private:
	Scene m_scene;

	double m_next_reload { 1.0 };
public:
	void on_init() override {
		m_scene.m_renderer->new_shader("test shader",
			ResourceManager::load_string("shaders/test.glsl"));

		Entity e = m_scene.new_entity();
		e.add_component<Shader>(m_scene.m_renderer->get_shader("test shader"));
		e.add_component<Mesh>(m_scene.m_renderer->new_sphere_mesh("sphere", 1.0));
	}

	void on_update() override {
		m_scene.render();

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
