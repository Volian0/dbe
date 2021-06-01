#include <engine.hpp>
#include <imgui.h>
#include <imgui_internal.h>

static std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

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

		if (ImGui::TreeNodeEx((void*)(u64)entity, flags, "%s", tag.name.c_str())) {
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

	std::shared_ptr <RenderTarget> m_scene_fb;

	std::string m_current_display_dir;
	std::string m_selected_file;
public:
	void on_init() override {
		m_input_manager = std::make_shared<InputManager>(*m_window);

		m_scene_fb = std::make_shared<RenderTarget>(vec2(1024, 768));

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

		m_scene.render(vec2{(float)m_window->width, (float)m_window->height},
			m_scene_fb.get());

		GUI::begin_frame();
			ImGui::Begin(ICON_FK_TH_LIST " hierarchy");
				m_hierarchy_renderer->render();
			ImGui::End();

			ImGui::Begin(ICON_FK_COG " properties");
				if (m_selected_file.size() != 0 &&
					ResourceManager::get_file_extension(m_selected_file) == "glsl" ||
					ResourceManager::get_file_extension(m_selected_file) == "txt" ||
					ResourceManager::get_file_extension(m_selected_file) == "cpp" ||
					ResourceManager::get_file_extension(m_selected_file) == "hpp" ||
					ResourceManager::get_file_extension(m_selected_file) == "h" ||
					ResourceManager::get_file_extension(m_selected_file) == "c" ||
					ResourceManager::get_file_extension(m_selected_file) == "md") {
					std::string path = m_current_display_dir + m_selected_file;
					ImGui::Text("File preview: res/%s", path.c_str());

					ImGui::BeginChild("preview",
						ImVec2(ImGui::GetWindowContentRegionWidth(), 300));
					ImGui::TextWrapped("%s", ResourceManager::load_string(path).c_str());
					ImGui::EndChild();
				}
			ImGui::End();

			ImGui::Begin(ICON_FK_TH_LIST " resource manager");
				if (ImGui::Button("res")) {
					m_current_display_dir = "";
				}
				ImGui::SameLine();
				ImGui::Text("/");

				auto path = split(m_current_display_dir, "/");
				std::string uptil;
				for (const auto& section : path) {
					ImGui::SameLine();

					uptil += section + "/";

					if (ImGui::Button(section.c_str())) {
						m_current_display_dir = uptil;
					}

					ImGui::SameLine();
					ImGui::Text("/");
				}

				ImGui::Separator();

				auto dir_list = ResourceManager::get_dir(m_current_display_dir);

				for (const auto& entry : dir_list) {
					auto name = entry.name;

					auto i = name.find_last_of('/');
					std::string end;
					if (i != std::string::npos) {
						end = name.substr(i + 1);
					}
					name = end;

					const char* icon = entry.is_directory ? ICON_FK_FOLDER : ICON_FK_FILE;

					ImGui::BeginGroup();
						ImVec4 color = { 1.0, 1.0, 1.0, 1.0};
						if (m_selected_file == name) {
							color = ImVec4(0.391f, 0.391f, 1.000f, 0.781f);
						}

						ImGui::PushStyleColor(ImGuiCol_Text, color);

						ImVec2 text_size = ImGui::CalcTextSize(name.c_str());
						ImVec2 icon_size = ImGui::CalcTextSize(icon);

						float width = std::max(text_size.x, 65.0f);

						ImVec2 before_button = ImGui::GetCursorPos();
						if (ImGui::Button(std::string("##" + name).c_str(),
							ImVec2(width, ((text_size.y + icon_size.y) * 2) + 7)) &&
							!entry.is_directory) {
							m_selected_file = name;
						}

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)
							&& entry.is_directory) {
							m_current_display_dir += name + "/";
						}

						ImGui::SetCursorPos(before_button);

						ImGui::SetCursorPosX(
							(ImGui::GetCursorPosX() + (text_size.x / 2))
							- (icon_size.x / 2) - 12);

						GUI::use_big_icons();
						ImGui::Text(icon);
						GUI::use_small_icons();

						ImGui::Text("%s", name.c_str());

						ImGui::PopStyleColor();
					ImGui::EndGroup();

					ImGui::SameLine();
				}

				if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() &&
					ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					m_selected_file = "";
				}

			ImGui::End();

			ImGui::Begin(ICON_FK_CUBE " scene");
				ImVec2 window_size = ImGui::GetWindowSize();
				window_size.x -= 20;
				window_size.y -= 40;

				m_scene_fb->resize(vec2(window_size.x, window_size.y));

				m_scene.m_renderer->m_projection = mat4::persp(75.0,
					(float)window_size.x/(float)window_size.y,
					0.1, 100.0);

				ImVec2 uv_min = ImVec2(0.0f, 0.0f);
				ImVec2 uv_max = ImVec2(1.0f, -1.0f);

				ImGui::Image((void*)(u64)m_scene_fb->get_output(),
					ImVec2(m_scene_fb->get_size().x, m_scene_fb->get_size().y),
					uv_min, uv_max);
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
		1024, 768,
		"Sandbox",
		false
	});
}
