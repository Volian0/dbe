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

class Renderer : public System {
private:
	std::map<std::string, Shader> m_shaders;
public:
	void render() const;

	Shader new_shader(const std::string& name, const std::string& source);
	Shader get_shader(const std::string& name);
	void bind_shader(const Shader& shader) const;
	void set_shader_uniform_int(const Shader& shader, const std::string& name, i32 val) const;
	void set_shader_uniform_float(const Shader& shader, const std::string& name, float val) const;
	void set_shader_uniform_vec2(const Shader& shader, const std::string& name, const vec2& val) const;
	void set_shader_uniform_vec3(const Shader& shader, const std::string& name, const vec3& val) const;
	void set_shader_uniform_vec4(const Shader& shader, const std::string& name, const vec4& val) const;
	void set_shader_uniform_mat4(const Shader& shader, const std::string& name, const mat4& val) const;

	~Renderer();
};
