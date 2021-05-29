#begin VERTEX

#version 430 core

layout (location = 0) in vec3 a_position;

uniform mat4 u_transform = mat4(1.0);
uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);

void main() {
	gl_Position = u_view * u_projection * u_transform * vec4(a_position, 1.0);
}

#end VERTEX

#begin FRAGMENT

#version 430 core

void main() {}
#end FRAGMENT
