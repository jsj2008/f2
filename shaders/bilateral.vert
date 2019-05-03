#version 330 core

layout (location = 0) in vec2 a_pos;

out vec2 uv;

void main() {
    vec2 pos = a_pos * 2.f - vec2(1.f);
    gl_Position = vec4(pos.x, pos.y, 0.f, 1.f);
    uv = a_pos;
}