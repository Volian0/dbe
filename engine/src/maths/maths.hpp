#pragma once

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.hpp"

#if !defined(PI)
#define PI 3.14159265358f
#endif

float to_rad(float deg);
vec2 to_rad(const vec2& deg);
vec3 to_rad(const vec3& deg);
vec4 to_rad(const vec4& deg);

float to_deg(float rad);
vec2 to_deg(const vec2& rad);
vec3 to_deg(const vec3& rad);
vec4 to_deg(const vec4& rad);
