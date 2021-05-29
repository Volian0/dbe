#begin VERTEX

#version 430 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_uv;

out vec2 v_uv;

void main() {
	v_uv = a_uv;
	gl_Position = vec4(a_position, 0.0, 1.0);
}

#end VERTEX

#begin FRAGMENT

#version 430 core

out vec4 v_color;
in vec2 v_uv;

uniform sampler2D u_input;
uniform sampler2D u_depth;

uniform float u_width;
uniform float u_height;

void main() {
	vec4 color = texture2D(u_input, v_uv);

	float depth = texture2D(u_depth, v_uv).r;

	float scale = 2;

	float half_scale_floor = floor(scale * 0.5);
	float half_scale_ceil = ceil(scale * 0.5);

	vec2 texel_size = vec2(1.0 / u_width, 1.0 / u_height);

	vec2 bottom_left = v_uv - texel_size * half_scale_floor;
	vec2 top_right = v_uv + texel_size * half_scale_ceil;
	vec2 bottom_right = v_uv + vec2(texel_size.x * half_scale_ceil,
		-texel_size.y * half_scale_floor);
	vec2 top_left = v_uv + vec2(-texel_size.x * half_scale_floor,
		texel_size.y * half_scale_ceil);

	float depth0 = texture2D(u_depth, bottom_left).r;
	float depth1 = texture2D(u_depth, top_right).r;
	float depth2 = texture2D(u_depth, bottom_right).r;
	float depth3 = texture2D(u_depth, top_left).r;

	float depth_finite_difference0 = depth1 - depth0;
	float depth_finite_difference1 = depth3 - depth2;

	float edge_depth = sqrt(pow(depth_finite_difference0, 2)
		+ pow(depth_finite_difference1, 2)) * 100;

	edge_depth = edge_depth > 1.5 ? 1.0 : 0.0;

	if (edge_depth > 0.2) {
		color = vec4(0.0);
	}

	v_color = color;
}

#end FRAGMENT
