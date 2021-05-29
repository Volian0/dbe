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

vec2 texel_size = vec2(1.0 / u_width, 1.0 / u_height);
vec3 luma = vec3(0.299, 0.587, 0.114);

vec4 do_fxaa(vec4 color) {
	float FXAA_SPAN_MAX = 4.0;
	float FXAA_REDUCE_MIN = 1.0/128.0;
	float FXAA_REDUCE_MUL = 1.0/80.0;

	float luma_TL = dot(luma, texture2D(u_input, v_uv.xy + (vec2(-1.0, -1.0) * texel_size)).rgb);
	float luma_TR = dot(luma, texture2D(u_input, v_uv.xy + (vec2(1.0, -1.0) * texel_size)).rgb);
	float luma_BL = dot(luma, texture2D(u_input, v_uv.xy + (vec2(-1.0, 1.0) * texel_size)).rgb);
	float luma_BR = dot(luma, texture2D(u_input, v_uv.xy + (vec2(1.0, 1.0) * texel_size)).rgb);
	float luma_M  = dot(luma, texture2D(u_input, v_uv.xy).rgb);

	vec2 dir;
	dir.x = -((luma_TL + luma_TR) - (luma_BL + luma_BR));
	dir.y = ((luma_TL + luma_BL) - (luma_TR + luma_BR));

	float dir_reduce = max((luma_TL + luma_TR + luma_BL + luma_BR)
		* (FXAA_REDUCE_MUL * 0.25), FXAA_REDUCE_MIN);
	float inverse_dir_adjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dir_reduce);

	dir = min(vec2(FXAA_SPAN_MAX),
		max(vec2(-FXAA_SPAN_MAX), dir * inverse_dir_adjustment)) * texel_size;

	vec3 result1 = (1.0 / 2.0) * (
		texture2D(u_input, v_uv + (dir * vec2(1.0 / 3.0 - 0.5))).rgb +
		texture2D(u_input, v_uv + (dir * vec2(2.0 / 3.0 - 0.5))).rgb);

	vec3 result2 = result1 * (1.0 / 2.0 ) + (1.0 / 4.0) * (
		texture2D(u_input, v_uv + (dir * vec2(0.0 / 3.0 - 0.5))).rgb +
		texture2D(u_input, v_uv + (dir * vec2(3.0 / 3.0 - 0.5))).rgb);

	float luma_min = min(luma_M, min(min(luma_TL, luma_TR), min(luma_BL, luma_BR)));
	float luma_max = max(luma_M, max(max(luma_TL, luma_TR), max(luma_BL, luma_BR)));
	float luma_result2 = dot(luma, result2);

	if (luma_result2 < luma_min || luma_result2 > luma_max) {
		return vec4(result1, 1.0);
	}

	return vec4(result2, 1.0);
}

vec4 do_outline(vec4 color, float thickness) {
	float depth = texture2D(u_depth, v_uv).r;

	float scale = thickness;

	float half_scale_floor = floor(scale * 0.5);
	float half_scale_ceil = ceil(scale * 0.5);

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
		return vec4(0.0, 0.0, 0.0, 1.0);
	}

	return color;
}

void main() {
	vec4 color = do_fxaa(texture2D(u_input, v_uv));

	v_color = do_outline(color, 2.0);
}

#end FRAGMENT
