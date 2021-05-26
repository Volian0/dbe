#include <engine.hpp>

std::string test_source = R"(
#begin VERTEX
#version 430 core

layout (location = 0) in vec3 position;

uniform mat4 transform = mat4(1.0);
uniform mat4 projection = mat4(1.0);
uniform mat4 view = mat4(1.0);

void main() {
	gl_Position = view * projection * transform * vec4(position, 1.0);
}
#end VERTEX

#begin FRAGMENT
#version 430 core

out vec4 color;

void main() {
	color = vec4(1.0);
}

#end FRAGMENT
)";

class Sandbox : public Application {
private:
	Scene m_scene;
public:
	void on_init() override {
		m_scene.m_renderer->new_shader("test shader", test_source);

		Entity e = m_scene.new_entity();
		e.add_component<Shader>(m_scene.m_renderer->get_shader("test shader"));
	}

	void on_update() override {
		m_scene.render();
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
