#pragma once

#include <string>
#include <map>

/* All the rendering happens here
 * This is simply a system that gets registered
 * by the ECS. */

#include "system/entity/ecs.hpp"
#include "int.hpp"
#include "maths/maths.hpp"

struct Shader {
	u32 id;
};

struct Mesh {
	u32 va;
	u32 vb;
	u32 ib;
	u32 index_count;
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
	std::map <std::string, Shader> m_shaders;
	std::map <std::string, Mesh> m_meshes;

public:
	mat4 m_projection;
	
	void render(ECS& ecs) const;

	Shader new_shader(const std::string& name, const std::string& source);
	Shader get_shader(const std::string& name);
	void delete_shader(const Shader& shader);
	void bind_shader(const Shader& shader) const;
	void set_shader_uniform_int(const Shader& shader, const std::string& name, i32 val) const;
	void set_shader_uniform_float(const Shader& shader, const std::string& name, float val) const;
	void set_shader_uniform_vec2(const Shader& shader, const std::string& name, const vec2& val) const;
	void set_shader_uniform_vec3(const Shader& shader, const std::string& name, const vec3& val) const;
	void set_shader_uniform_vec4(const Shader& shader, const std::string& name, const vec4& val) const;
	void set_shader_uniform_mat4(const Shader& shader, const std::string& name, const mat4& val) const;

	Mesh new_mesh(const std::string& name,
		const std::vector <float>& vertices, const std::vector <u32>& indices,
		const std::vector <MeshLayoutConfig>& layout_config);
	Mesh new_sphere_mesh(const std::string& name, float radius);
	Mesh get_mesh(const std::string& name);
	void delete_mesh(const Mesh& mesh);

	~Renderer();
};
