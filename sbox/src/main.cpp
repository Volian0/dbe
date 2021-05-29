#include <engine.hpp>

class Sandbox : public Application {
private:
	Scene m_scene;

	double m_next_reload { 1.0 };

	Entity sphere_entity;
	Entity sphere_entity2;

	std::shared_ptr<InputManager> m_input_manager;
public:
	void on_init() override {
		m_input_manager = std::make_shared<InputManager>(*m_window);

		m_scene.m_renderer->new_shader("postprocess",
			ResourceManager::load_string("shaders/postprocess.glsl"));

		m_scene.m_renderer->new_shader("test shader",
			ResourceManager::load_string("shaders/test.glsl"));

		m_scene.m_renderer->init("postprocess");

		sphere_entity = m_scene.new_entity();
		sphere_entity.add_component<Transform>({
			{0.0, 0.0, -3.0}, /* translation */
			{0.0, 0.0, 0.0}, /* rotation */
			{1.0, 1.0, 1.0}}); /* scale */
		sphere_entity.add_component<Shader>(m_scene.m_renderer->get_shader("test shader"));
		sphere_entity.add_component<Mesh>(m_scene.m_renderer->new_sphere_mesh("sphere",
			Mesh::Flags::DRAW_TRIANGLES, 1.0));
		sphere_entity.add_component<Material>({
			{0.788, 0.176, 0.133}, /* lit color */
			{0.388, 0.141, 0.121}, /* unlit color */
			{0.886, 0.557, 0.533}, /* specular color */
			0.9, /* specular_cutoff */
		});

		sphere_entity2 = m_scene.new_entity();
		sphere_entity2.add_component<Transform>({
			{1.0, 0.0, -3.0}, /* translation */
			{0.0, 0.0, 0.0}, /* rotation */
			{1.0, 1.0, 1.0}}); /* scale */
		sphere_entity2.add_component<Shader>(m_scene.m_renderer->get_shader("test shader"));
		sphere_entity2.add_component<Mesh>(m_scene.m_renderer->new_sphere_mesh("sphere",
			Mesh::Flags::DRAW_TRIANGLES, 1.0));
		sphere_entity2.add_component<Material>({
			{0.196, 0.235, 0.821}, /* lit color */
			{0.055, 0.082, 0.447}, /* unlit color */
			{0.445, 0.588, 0.898}, /* specular color */
			0.9, /* specular_cutoff */
		});

		m_scene.m_light_renderer->m_sun.direction = { 0.5, -1.0, -1.0 };
	}

	void on_update() override {
		//handle input
		m_input_manager->update();

		if (m_input_manager->key_state(GLFW_KEY_ESCAPE) == KeyState::PRESSED)
		{
			return m_window->close();
		}

		auto& sphere_translation = sphere_entity.get_component<Transform>().translation;
		if (m_input_manager->is_held(GLFW_KEY_W))
			sphere_translation.y += m_window->timestep;
		if (m_input_manager->is_held(GLFW_KEY_S))
			sphere_translation.y -= m_window->timestep;
		if (m_input_manager->is_held(GLFW_KEY_D))
			sphere_translation.x += m_window->timestep;
		if (m_input_manager->is_held(GLFW_KEY_A))
			sphere_translation.x -= m_window->timestep;

		m_scene.m_renderer->m_projection = mat4::persp(75.0,
			(float)m_window->width/(float)m_window->height,
			0.1, 100.0);

		m_scene.render(vec2{m_window->width, m_window->height});

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
