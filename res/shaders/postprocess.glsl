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

void make_kernel(inout vec4 n[9], sampler2D tex, vec2 coord) {
	float w = 1.0 / u_width;
	float h = 1.0 / u_height;

	n[0] = texture2D(tex, coord + vec2( -w, -h));
	n[1] = texture2D(tex, coord + vec2(0.0, -h));
	n[2] = texture2D(tex, coord + vec2(  w, -h));
	n[3] = texture2D(tex, coord + vec2( -w, 0.0));
	n[4] = texture2D(tex, coord);
	n[5] = texture2D(tex, coord + vec2(  w, 0.0));
	n[6] = texture2D(tex, coord + vec2( -w, h));
	n[7] = texture2D(tex, coord + vec2(0.0, h));
	n[8] = texture2D(tex, coord + vec2(  w, h));
}

void main() {
	vec4 color = texture2D(u_input, v_uv);

	vec4 n[9];
	make_kernel(n, u_input, v_uv);

	vec4 sobel_edge_h = n[2] + (2.0*n[5]) + n[8] - (n[0] + (2.0*n[3]) + n[6]);
	vec4 sobel_edge_v = n[0] + (2.0*n[1]) + n[2] - (n[6] + (2.0*n[7]) + n[8]);
	vec4 sobel = sqrt((sobel_edge_h * sobel_edge_h) + (sobel_edge_v * sobel_edge_v));

	if (sobel.r > 0.1) {
		v_color = vec4(0.0, 0.0, 0.0, 1.0);
	} else {
		v_color = color;
	}
}

#end FRAGMENT
