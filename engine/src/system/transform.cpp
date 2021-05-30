#include "transform.hpp"
#include "scene.hpp"

mat4 get_transform_matrix(const Transform& transform) {
	mat4 result(1.0f);

	result *= mat4::translate(transform.translation);

	mat4 rotation_matrix(1.0f);
	rotation_matrix *= mat4::rotate(transform.rotation.x, vec3(1.0f, 0.0f, 0.0f));
	rotation_matrix *= mat4::rotate(transform.rotation.y, vec3(0.0f, 1.0f, 0.0f));
	rotation_matrix *= mat4::rotate(transform.rotation.z, vec3(0.0f, 0.0f, 1.0f));

	result *= rotation_matrix;
	result *= mat4::scale(transform.scale);

	if (transform.parent && transform.parent->has_component<Transform>()) {
		result = get_transform_matrix(transform.parent->get_component<Transform>()) * result;
	}

	return result;
}
