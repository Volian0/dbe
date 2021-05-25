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

	~Renderer();
};
