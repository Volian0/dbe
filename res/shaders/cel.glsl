#begin VERTEX
#version 430 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

uniform mat4 u_transform = mat4(1.0);
uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);

out vec3 v_normal;
out vec3 v_frag_pos;

void main() {
	v_normal = mat3(transpose(inverse(u_transform))) * a_normal;
	v_frag_pos = vec3(u_transform * vec4(a_position, 1.0));

	gl_Position = u_view * u_projection * u_transform * vec4(a_position, 1.0);
}

#end VERTEX

#begin FRAGMENT
#version 430 core

struct Sun {
	vec3 direction;
	vec3 color;
	float intensity;
};

struct Material {
	vec3 lit_color;
	vec3 unlit_color;
	vec3 specular_color;
	float specular_cutoff;
};

uniform Material u_material;

uniform Sun u_sun;
uniform vec3 u_camera_position;

out vec4 v_color;
in vec3 v_normal;
in vec3 v_frag_pos;

void main() {
	Sun light = u_sun;
	vec3 normal = normalize(v_normal);
	vec3 view_dir = normalize(u_camera_position - v_frag_pos);

	vec3 light_dir = normalize(-light.direction);

	float diff = max(dot(normal, light_dir), 0.0);

	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = max(dot(view_dir, reflect_dir), 0.0);

	vec3 final = vec3(0.0);

	float intensity = 0.6 * diff + 0.4 * spec;
	if (intensity > u_material.specular_cutoff) {
		final = u_material.specular_color;
	} else if (intensity > 0.2) {
		final = u_material.lit_color;
	} else {
		final = u_material.unlit_color;
	}

	v_color = vec4(final, 1.0);
}

#end FRAGMENT
