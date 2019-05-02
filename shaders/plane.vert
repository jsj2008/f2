#version 330 core

layout(location = 0) in vec3 a_pos;

out vec3 f_pos;

uniform vec3 w_pos;
uniform vec2 dim;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    vec4 pos = model * vec4(a_pos, 1.f);;
    f_pos = pos.xyz;
	  gl_Position = proj * view * pos;
}
