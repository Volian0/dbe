#pragma once

#include "maths/maths.hpp"

struct Transform {
	vec3 translation { 0.0, 0.0, 0.0 };
	vec3 rotation { 0.0, 0.0, 0.0 };
	vec3 scale { 1.0, 1.0, 1.0 };
};

mat4 get_transform_matrix(const Transform& transform);
