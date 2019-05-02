#version 330 core

layout(location=0) in vec3 a_pos;

flat out vec4 f_pos;

uniform ivec2 vp;
uniform float radius;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    f_pos = model * vec4(a_pos, 1.f);
	  gl_Position = proj * view * f_pos;
    gl_PointSize = vp.y * proj[1][1] * radius / gl_Position.w;
}
