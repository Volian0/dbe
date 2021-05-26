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
	if (m_shaders.count(name) != 0) {
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

	Shader shader = { id };
	m_shaders[name] = shader;
	return shader;
}

Shader Renderer::get_shader(const std::string& name) {
	if (m_shaders.count(name) == 0) {
		log(LOG_ERROR, "A shader with the name `%s' doesn't exist.", name.c_str());
		return { UINT32_MAX };
	}

	return m_shaders[name];
}

void Renderer::bind_shader(const Shader& shader) const {
	glUseProgram(shader.id);
}

void Renderer::set_shader_uniform_int(const Shader& shader, const std::string& name, i32 val) const {
	u32 location = glGetUniformLocation(shader.id, name.c_str());
	glUniform1i(location, val);
}

void Renderer::set_shader_uniform_float(const Shader& shader, const std::string& name, float val) const {
	u32 location = glGetUniformLocation(shader.id, name.c_str());
	glUniform1f(location, val);
}

void Renderer::set_shader_uniform_vec2(const Shader& shader, const std::string& name, const vec2& val) const {
	u32 location = glGetUniformLocation(shader.id, name.c_str());
	glUniform2f(location, val.x, val.y);
}

void Renderer::set_shader_uniform_vec3(const Shader& shader, const std::string& name, const vec3& val) const {
	u32 location = glGetUniformLocation(shader.id, name.c_str());
	glUniform3f(location, val.x, val.y, val.z);
}

void Renderer::set_shader_uniform_vec4(const Shader& shader, const std::string& name, const vec4& val) const {
	u32 location = glGetUniformLocation(shader.id, name.c_str());
	glUniform4f(location, val.x, val.y, val.z, val.w);
}

void Renderer::set_shader_uniform_mat4(const Shader& shader, const std::string& name, const mat4& val) const {
	u32 location = glGetUniformLocation(shader.id, name.c_str());
	glUniformMatrix4fv(location, 1, GL_TRUE, val.elements);
}
