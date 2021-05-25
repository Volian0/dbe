#include <math.h>

#include "maths.hpp"

float to_rad(float deg) {
	return (float)(deg * (PI / 180.0f));
}

vec2 to_rad(const vec2& deg) {
	return vec2(to_rad(deg.x), to_rad(deg.y));
}

vec3 to_rad(const vec3& deg) {
	return vec3(to_rad(deg.x), to_rad(deg.y), to_rad(deg.z));

}

vec4 to_rad(const vec4& deg) {
	return vec4(to_rad(deg.x), to_rad(deg.y), to_rad(deg.z), to_rad(deg.w));
}

float to_deg(float rad) {
	return (float)(rad * (180.0f / PI));
}

vec2 to_deg(const vec2& rad) {
	return vec2(to_deg(rad.x), to_deg(rad.y));
}

vec3 to_deg(const vec3& rad) {
	return vec3(to_deg(rad.x), to_deg(rad.y), to_deg(rad.z));
}

vec4 to_deg(const vec4& rad) {
	return vec4(to_deg(rad.x), to_deg(rad.y), to_deg(rad.z), to_deg(rad.w));
}
