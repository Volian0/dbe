#pragma once

#include "maths/maths.hpp"

class Entity;

struct Transform {
	vec3 translation { 0.0, 0.0, 0.0 };
	vec3 rotation { 0.0, 0.0, 0.0 };
	vec3 scale { 1.0, 1.0, 1.0 };

	Entity* parent { nullptr };
};

mat4 get_parent_matrix(const Transform& transform, mat4 current = mat4(1.0));
mat4 get_transform_matrix(const Transform& transform);

