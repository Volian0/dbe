#include <math.h>

#include <tuple>
#include <sstream>

#include <glad/glad.h>

#include "renderer.hpp"
#include "logger.hpp"
#include "system/transform.hpp"

Renderer::~Renderer() {
	for (const auto& s : m_shaders) {
		delete_shader(s.second);
	}

	for (const auto& m : m_meshes) {
		delete_mesh(m.second);
	}
}

void Renderer::render(ECS& ecs) const {
	for (const auto& entity : m_entities) {
		auto& transform = ecs.get_component<Transform>(entity);
		auto& shader = ecs.get_component<Shader>(entity);
		auto& mesh = ecs.get_component<Mesh>(entity);

		bind_shader(shader);
		set_shader_uniform_mat4(shader, "projection", m_projection);
		set_shader_uniform_mat4(shader, "transform", get_transform_matrix(transform));

		glBindVertexArray(mesh.va);
		glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
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
	glShaderSource(f, 1, &fs_ptr, nullptr);
	glCompileShader(f);

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

void Renderer::delete_shader(const Shader& shader) {
	glDeleteProgram(shader.id);
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

Mesh Renderer::new_mesh(const std::string& name,
		const std::vector <float>& vertices, const std::vector <u32>& indices,
		const std::vector <MeshLayoutConfig>& layout_config) {
	if (m_meshes.count(name) != 0) {
		log(LOG_WARNING, "A mesh with the name `%s' already exists", name.c_str());
		return m_meshes[name];
	}

	Mesh result;

	glGenVertexArrays(1, &result.va);
	glGenBuffers(1, &result.vb);
	glGenBuffers(1, &result.ib);

	glBindVertexArray(result.va);
	glBindBuffer(GL_ARRAY_BUFFER, result.vb);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() *
			sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() *
			sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	for (const auto& mlc : layout_config) {
		glVertexAttribPointer(
			mlc.index,
			mlc.component_count,
			GL_FLOAT, GL_FALSE,
			mlc.stride * sizeof(float),
			(void*)(u64)(mlc.start_offset * sizeof(float)));
		glEnableVertexAttribArray(mlc.index);
	}

	glBindVertexArray(0);

	m_meshes[name] = result;
	return result;
}

Mesh Renderer::new_sphere_mesh(const std::string& name, float radius) {
	float sectorCount = 36.0f;
	float stackCount = 18.0f;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float x, y, z, xy;
	float nx, ny, nz, lengthInv = 1.0f / radius;

	float s, t;

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for(int i = 0; i <= stackCount; ++i) {
		int k1 = i * (sectorCount + 1);
		int k2 = k1 + sectorCount + 1;


		stackAngle = PI / 2 - i * stackStep;
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);

		for(int j = 0; j <= sectorCount; ++j, ++k1, ++k2) {
			sectorAngle = j * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			s = (float)j / sectorCount;
			t = (float)i / stackCount;

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			vertices.push_back(nx);
			vertices.push_back(ny);
			vertices.push_back(nz);
			vertices.push_back(s);
			vertices.push_back(t);

			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}


			if (i != (stackCount-1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	std::vector <MeshLayoutConfig> mlc = {
		{
			0, 3, 8, 0
		},
		{
			1, 3, 8, 3
		},
		{
			2, 2, 8, 6
		},
	};

	return new_mesh(name, vertices, indices, mlc);
}

Mesh Renderer::get_mesh(const std::string& name) {
	if (m_meshes.count(name) == 0) {
		log(LOG_ERROR, "A mesh with the name `%s' doesn't exist.", name.c_str());
		return {
			UINT32_MAX,
			UINT32_MAX,
			UINT32_MAX,
			UINT32_MAX
		};
	}

	return m_meshes[name];
}

void Renderer::delete_mesh(const Mesh& mesh) {
	glDeleteVertexArrays(1, &mesh.va);
	glDeleteBuffers(1, &mesh.vb);
	glDeleteBuffers(1, &mesh.ib);
}
