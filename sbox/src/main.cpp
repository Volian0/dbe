#include <engine.hpp>
#include <imgui.h>


class EntityHierarchyRenderer : public System {
private:
public:
	void render_entity(EntityHandle entity) {
		const auto& tag = m_ecs->get_component<Tag>(entity);
		const auto& hierarchy = m_ecs->get_component<Hierarchy>(entity);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

		if (hierarchy.children.size() <= 0) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		if (ImGui::TreeNodeEx((void*)(u32)entity, flags, "%s", tag.name.c_str())) {
			for (const auto& child : hierarchy.children) {
				render_entity(child);
			}

			ImGui::TreePop();
		}
	}

	void render() {
		for (const auto& entity : m_entities) {
			const auto& hierarchy = m_ecs->get_component<Hierarchy>(entity);

			if (hierarchy.parent == NULL_ENTITY) {
				render_entity(entity);
			}
		}
	}
};

class Sandbox : public Application {
private:
	Scene m_scene;

	double m_next_reload { 1.0 };

	Entity monkey;

	std::shared_ptr<InputManager> m_input_manager;
	std::shared_ptr <EntityHierarchyRenderer> m_hierarchy_renderer;
public:
	void on_init() override {
		m_input_manager = std::make_shared<InputManager>(*m_window);

		GUI::init(m_window);

		m_scene.m_renderer->new_shader("postprocess",
			ResourceManager::load_string("shaders/postprocess.glsl"));

		m_scene.m_renderer->new_shader("depth",
			ResourceManager::load_string("shaders/depth.glsl"));

		m_scene.m_renderer->new_shader("cel",
			ResourceManager::load_string("shaders/cel.glsl"));

		m_scene.m_renderer->init("postprocess", "depth");

		{
			m_hierarchy_renderer = m_scene.m_ecs.register_system<EntityHierarchyRenderer>();
			Signature sig;
			sig.set(m_scene.m_ecs.get_component_type<Tag>());
			sig.set(m_scene.m_ecs.get_component_type<Hierarchy>());
			m_scene.m_ecs.set_system_signature<EntityHierarchyRenderer>(sig);
		}

		monkey = load_model(m_scene, "monkey.glb", "cel");
		monkey.get_component<Transform>().translation.z = -2.0;
		monkey.get_component<Tag>().name = "monkey";

		m_scene.m_light_renderer->m_sun.direction = { 0.5, -1.0, -1.0 };
	}

	void on_update() override {
		//handle input
		m_input_manager->update();

		if (m_input_manager->key_state(GLFW_KEY_ESCAPE) == KeyState::PRESSED)
		{
			return m_window->close();
		}

		monkey.get_component<Transform>().rotation.y += 25.0 * m_window->timestep;

		m_scene.update();

		m_scene.m_renderer->m_projection = mat4::persp(75.0,
			(float)m_window->width/(float)m_window->height,
			0.1, 100.0);

		m_scene.render(vec2{(float)m_window->width, (float)m_window->height});

		GUI::begin_frame();
			ImGui::Begin("hierarchy");
				m_hierarchy_renderer->render();
			ImGui::End();
		GUI::end_frame();

		m_next_reload -= m_window->timestep;
		if (m_next_reload <= 0.0) {
			m_next_reload = 1.0;
			ResourceManager::hot_reload();
		}
	}

	void on_destroy() override {
		GUI::quit();
	}
};

int main() {
	Sandbox sbox;
	sbox.run({
		1366, 768,
		"Sandbox",
		false
	});
}
