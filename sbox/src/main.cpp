#include <engine.hpp>

class Sandbox : public Application {
private:
	Scene m_scene;

	double m_next_reload { 1.0 };

	mat4 m_projection;

	Entity sphere_entity;
public:
	void on_init() override {
		m_scene.m_renderer->new_shader("test shader",
			ResourceManager::load_string("shaders/test.glsl"));

		sphere_entity = m_scene.new_entity();
		sphere_entity.add_component<Transform>({{0.0, 0.0, -3.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}});
		sphere_entity.add_component<Shader>(m_scene.m_renderer->get_shader("test shader"));
		sphere_entity.add_component<Mesh>(m_scene.m_renderer->new_sphere_mesh("sphere", 1.0));
	}

	void on_update() override {
		m_projection = mat4::persp(75.0,
			(float)m_window->width/(float)m_window->height,
			0.1, 100.0);

		mat4 transform = mat4::translate({0.0, 0.0, -3.0});

		/* TODO: Projection should be part of the renderer */
		Shader s = m_scene.m_renderer->get_shader("test shader");
		m_scene.m_renderer->bind_shader(s);
		m_scene.m_renderer->set_shader_uniform_mat4(s, "projection", m_projection);

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
