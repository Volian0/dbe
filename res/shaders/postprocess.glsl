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

	float depth0 = texture2D(u_depth, vec2(v_uv.x - 0.002, v_uv.y - 0.002)).r;
	float depth1 = texture2D(u_depth, vec2(v_uv.x + 0.002, v_uv.y - 0.002)).r;
	float depth2 = texture2D(u_depth, vec2(v_uv.x + 0.002, v_uv.y + 0.002)).r;
	float depth3 = texture2D(u_depth, vec2(v_uv.x - 0.002, v_uv.y + 0.002)).r;

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
