#include <tuple>
#include <sstream>

#include <glad/glad.h>

#include "renderer.hpp"
#include "logger.hpp"

Renderer::~Renderer() {
	for (const auto& s : m_shaders) {
		glDeleteProgram(s.second.id);
	}
}

void Renderer::render() const {

}

static std::pair<std::string, std::string> parse_shaders(const std::string& source) {
	std::stringstream v_source;
	std::stringstream f_source;

	std::stringstream ss(source);
	enum class ShaderType {
		NONE,
		VERTEX,
		FRAGMENT
	} current = ShaderType::NONE;

	for (std::string line; std::getline(ss, line);) {
		if (line.find("#begin VERTEX") != std::string::npos) {
			current = ShaderType::VERTEX;
		} else if (line.find("#begin FRAGMENT") != std::string::npos) {
			current = ShaderType::FRAGMENT;
		} else if (line.find("#end") != std::string::npos) {
			current = ShaderType::NONE;
		} else {
			switch (current) {
				case ShaderType::NONE:
					break;
				case ShaderType::VERTEX:
					v_source << line << '\n';
					break;
				case ShaderType::FRAGMENT:
					f_source << line << '\n';
					break;
				default:
					break;
			}
		}
	}

	return { v_source.str(), f_source.str() };
}

Shader Renderer::new_shader(const std::string& name, const std::string& source) {
	if (m_shaders.count(name)) {
		log(LOG_WARNING, "A shader with the name `%s' already exists.", name.c_str());
		return m_shaders[name];
	}

	auto parsed_source = parse_shaders(source);

	const char* vs_ptr = parsed_source.first.c_str();
	const char* fs_ptr = parsed_source.second.c_str();

	u32 v, f;
	v = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v, 1, &vs_ptr, nullptr);
	glCompileShader(v);

	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(v, 1, &fs_ptr, nullptr);
	glCompileShader(v);

	u32 id;
	id = glCreateProgram();
	glAttachShader(id, v);
	glAttachShader(id, f);
	glLinkProgram(id);

	glDeleteShader(v);
	glDeleteShader(f);

	return { id };
}

Shader Renderer::get_shader(const std::string& name) {
	if (!m_shaders.count(name)) {
		log(LOG_ERROR, "A shader with the name `%s' doesn't exist.", name.c_str());
		return { UINT32_MAX };
	}

	return m_shaders[name];
}

void Renderer::bind_shader(const Shader& shader) const {
	glUseProgram(shader.id);
}
