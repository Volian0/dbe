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


void Renderer::init(const std::string& postprocess_shader, const std::string& depth_shader) {
	m_postprocess = get_shader(postprocess_shader);
	m_depth = get_shader(depth_shader);

	m_rendertarget = std::make_shared<RenderTarget>(vec2{1024, 1024});
	m_depth_buffer = std::make_shared<DepthBuffer>(vec2{1024, 1024});

	std::vector<MeshLayoutConfig> mlc = {
		{
			0, 2, 4, 0
		},

		{
			1, 2, 4, 2
		}
	};

	m_fullscreen_quad = new_mesh("fullscreen quad", Mesh::Flags::DRAW_TRIANGLES,
		{
			/* position     UV */
			 1.0,  1.0, 	1.0f, 1.0f,
			 1.0, -1.0, 	1.0f, 0.0f,
			-1.0, -1.0, 	0.0f, 0.0f,
			-1.0,  1.0, 	0.0f, 1.0f
		},
		{
			0, 1, 3,
			1, 2, 3
		}, mlc
	);
}

void Renderer::render(const vec2& fb_size, RenderTarget* fb) {
	glClearColor(0.9, 0.9, 1.0, 1.0);

	vec2 size = fb_size;
	if (fb) {
		size = fb->get_size();
	}

	m_depth_buffer->resize(fb_size);
	m_depth_buffer->bind();

	bind_shader(m_depth);

	for (const auto& entity : m_entities) {
		auto& transform = m_ecs->get_component<Transform>(entity);
		auto& mesh = m_ecs->get_component<Mesh>(entity);

		set_shader_uniform_mat4(m_depth, "u_projection", m_projection);
		set_shader_uniform_mat4(m_depth, "u_transform", get_transform_matrix(transform));
		set_shader_uniform_vec3(m_depth, "u_camera_position", vec3(0.0, 0.0, 0.0));

		draw_mesh(mesh);
	}

	m_rendertarget->resize(fb_size);
	m_rendertarget->bind();

	for (const auto& entity : m_entities) {
		auto& transform = m_ecs->get_component<Transform>(entity);
		auto& mesh = m_ecs->get_component<Mesh>(entity);
		auto& material = m_ecs->get_component<Material>(entity);

		u32 shader = get_shader(m_ecs->get_component<Shader>(entity).name);

		bind_shader(shader);
		set_shader_uniform_mat4(shader, "u_projection", m_projection);
		set_shader_uniform_mat4(shader, "u_transform", get_transform_matrix(transform));
		set_shader_uniform_vec3(shader, "u_camera_position", vec3(0.0, 0.0, 0.0));

		set_shader_uniform_vec3(shader, "u_material.lit_color", material.lit_color);
		set_shader_uniform_vec3(shader, "u_material.unlit_color", material.unlit_color);
		set_shader_uniform_vec3(shader, "u_material.specular_color", material.specular_color);
		set_shader_uniform_float(shader, "u_material.specular_cutoff", material.specular_cutoff);

		draw_mesh(mesh);
	}

	m_rendertarget->unbind(fb_size);

	if (fb) {
		fb->bind();
	}

	m_rendertarget->bind_output(0);
	m_depth_buffer->bind_output(1);

	bind_shader(m_postprocess);
	set_shader_uniform_float(m_postprocess, "u_width", fb_size.x);
	set_shader_uniform_float(m_postprocess, "u_height", fb_size.y);
	set_shader_uniform_int(m_postprocess, "u_input", 0);
	set_shader_uniform_int(m_postprocess, "u_depth", 1);
	draw_mesh(m_fullscreen_quad);

	glBindVertexArray(0);

	if (fb) {
		fb->unbind(fb_size);
	}
}

void LightRenderer::init(Renderer* renderer) {
	m_renderer = renderer;
}

void LightRenderer::render() const {
	for (const auto& entity : m_entities) {
		u32 shader = m_renderer->get_shader(m_ecs->get_component<Shader>(entity).name);

		m_renderer->bind_shader(shader);

		m_renderer->set_shader_uniform_vec3(shader, "u_sun.direction", m_sun.direction);
		m_renderer->set_shader_uniform_vec3(shader, "u_sun.color", m_sun.color);
		m_renderer->set_shader_uniform_float(shader, "u_sun.intensity", m_sun.intensity);
	}
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

u32 Renderer::new_shader(const std::string& name, const std::string& source) {
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

	m_shaders[name] = id;
	return id;
}

u32 Renderer::get_shader(const std::string& name) {
	if (m_shaders.count(name) == 0) {
		log(LOG_ERROR, "A shader with the name `%s' doesn't exist.", name.c_str());
		return { UINT32_MAX };
	}

	return m_shaders[name];
}

void Renderer::delete_shader(u32 shader) {
	glDeleteProgram(shader);
}

void Renderer::bind_shader(u32 shader) const {
	glUseProgram(shader);
}

void Renderer::set_shader_uniform_int(u32 shader, const std::string& name, i32 val) const {
	u32 location = glGetUniformLocation(shader, name.c_str());
	glUniform1i(location, val);
}

void Renderer::set_shader_uniform_float(u32 shader, const std::string& name, float val) const {
	u32 location = glGetUniformLocation(shader, name.c_str());
	glUniform1f(location, val);
}

void Renderer::set_shader_uniform_vec2(u32 shader, const std::string& name, const vec2& val) const {
	u32 location = glGetUniformLocation(shader, name.c_str());
	glUniform2f(location, val.x, val.y);
}

void Renderer::set_shader_uniform_vec3(u32 shader, const std::string& name, const vec3& val) const {
	u32 location = glGetUniformLocation(shader, name.c_str());
	glUniform3f(location, val.x, val.y, val.z);
}

void Renderer::set_shader_uniform_vec4(u32 shader, const std::string& name, const vec4& val) const {
	u32 location = glGetUniformLocation(shader, name.c_str());
	glUniform4f(location, val.x, val.y, val.z, val.w);
}

void Renderer::set_shader_uniform_mat4(u32 shader, const std::string& name, const mat4& val) const {
	u32 location = glGetUniformLocation(shader, name.c_str());
	glUniformMatrix4fv(location, 1, GL_TRUE, val.elements);
}

Mesh Renderer::new_mesh(const std::string& name, const Mesh::Flags& flags,
		const std::vector <float>& vertices, const std::vector <u32>& indices,
		const std::vector <MeshLayoutConfig>& layout_config) {
	if (m_meshes.count(name) != 0) {
		log(LOG_WARNING, "A mesh with the name `%s' already exists", name.c_str());
		return m_meshes[name];
	}

	Mesh result;

	result.flags = flags;

	result.index_count = indices.size();

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

Mesh Renderer::new_sphere_mesh(const std::string& name, const Mesh::Flags& flags, float radius) {
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

	return new_mesh(name, flags, vertices, indices, mlc);
}

Mesh Renderer::new_cube_mesh(const std::string& name, const Mesh::Flags& flags) {
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

	return new_mesh(name, flags,
		std::vector<float> {
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.625, 0.500,
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.875, 0.500,
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.875, 0.750,
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.625, 0.500,
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.875, 0.750,
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.625, 0.750,
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.375, 0.750,
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.625, 0.750,
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.625, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.375, 0.750,
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.625, 1.0f,
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.375, 1.0f,
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.375, 0.0f,
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.625, 0.0f,
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.625, 0.250,
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.375, 0.0f,
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.625, 0.250,
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.375, 0.250,
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.125, 0.500,
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.375, 0.500,
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.375, 0.750,
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.125, 0.500,
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.375, 0.750,
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.125, 0.750,
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.375, 0.500,
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.625, 0.500,
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.625, 0.750,
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.375, 0.500,
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.625, 0.750,
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.375, 0.750,
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.375, 0.250,
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.625, 0.250,
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.625, 0.500,
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.375, 0.250,
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.625, 0.500,
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.375, 0.500,
		},
		std::vector<u32> {
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
		},
		mlc
	);
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

void Renderer::draw_mesh(const Mesh& mesh) {
	u32 draw_mode = GL_TRIANGLES;
	if ((u32)mesh.flags & (u32)Mesh::Flags::DRAW_LINES) {
		draw_mode = GL_LINES;
	} else if ((u32)mesh.flags & (u32)Mesh::Flags::DRAW_LINE_STRIP) {
		draw_mode = GL_LINE_STRIP;
	}

	glBindVertexArray(mesh.va);
	glDrawElements(draw_mode, mesh.index_count, GL_UNSIGNED_INT, 0);
}

void Renderer::delete_mesh(const Mesh& mesh) {
	glDeleteVertexArrays(1, &mesh.va);
	glDeleteBuffers(1, &mesh.vb);
	glDeleteBuffers(1, &mesh.ib);
}
