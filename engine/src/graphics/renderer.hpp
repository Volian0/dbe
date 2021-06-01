#pragma once

#include <string>
#include <map>

/* All the rendering happens here
 * This is simply a system that gets registered
 * by the ECS. */

#include "system/entity/ecs.hpp"
#include "int.hpp"
#include "maths/maths.hpp"
#include "rendertarget.hpp"
#include "depthbuffer.hpp"

struct Shader {
	std::string name;
};

struct Mesh {
	u32 va;
	u32 vb;
	u32 ib;
	u32 index_count;

	enum class Flags : u32 {
		DRAW_TRIANGLES = 1 << 0,
		DRAW_LINES = 1 << 1,
		DRAW_LINE_STRIP = 1 << 2
	} flags;
};

struct Material {
	vec3 lit_color;
	vec3 unlit_color;
	vec3 specular_color;
	float specular_cutoff;
};

/* Gets forwarded to glVertexAttribPointer
 * - defining how the shader should interpret
 * the vertex data. */
struct MeshLayoutConfig {
	u32 index;
	u32 component_count;
	u32 stride;
	u32 start_offset;
};

class Renderer : public System {
private:
	std::map <std::string, u32> m_shaders;
	std::map <std::string, Mesh> m_meshes;

	u32 m_postprocess;
	u32 m_depth;

	Mesh m_fullscreen_quad;

	std::shared_ptr <RenderTarget> m_rendertarget;
	std::shared_ptr <DepthBuffer> m_depth_buffer;
public:
	mat4 m_projection;

	void init(const std::string& postprocess_shader, const std::string& depth_shader);

	void render(const vec2& fb_size);

	u32 new_shader(const std::string& name, const std::string& source);
	u32 get_shader(const std::string& name);
	void delete_shader(u32 shader);
	void bind_shader(u32 shader) const;
	void set_shader_uniform_int(u32 shader, const std::string& name, i32 val) const;
	void set_shader_uniform_float(u32 shader, const std::string& name, float val) const;
	void set_shader_uniform_vec2(u32 shader, const std::string& name, const vec2& val) const;
	void set_shader_uniform_vec3(u32 shader, const std::string& name, const vec3& val) const;
	void set_shader_uniform_vec4(u32 shader, const std::string& name, const vec4& val) const;
	void set_shader_uniform_mat4(u32 shader, const std::string& name, const mat4& val) const;

	Mesh new_mesh(const std::string& name, const Mesh::Flags& flags,
		const std::vector <float>& vertices, const std::vector <u32>& indices,
		const std::vector <MeshLayoutConfig>& layout_config);
	Mesh new_sphere_mesh(const std::string& name, const Mesh::Flags& flags, float radius);
	Mesh new_cube_mesh(const std::string& name, const Mesh::Flags& flags);
	Mesh get_mesh(const std::string& name);
	void draw_mesh(const Mesh& mesh);
	void delete_mesh(const Mesh& mesh);

	~Renderer();
};

struct Sun {
	vec3 direction;
	vec3 color;
	float intensity;
};

class LightRenderer : public System {
private:
	Renderer* m_renderer { nullptr };
public:
	Sun m_sun { {0.5, -1.0, 0.0}, {1.0, 1.0, 1.0}, 1.0};

	void init(Renderer* renderer);

	void render() const;
};
