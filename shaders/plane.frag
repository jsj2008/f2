#version 330 core

in vec3 f_pos;

out vec4 f_color;

uniform vec3 eye;
uniform mat4 model;

void main() {
	vec3 ambient = vec3(0.1f);
	vec3 color = vec3(0.6f);

    ivec3 cell = ivec3(floor(f_pos.x / 0.5f), floor(f_pos.y / 0.5f), floor(f_pos.z / 0.5f));
    if ((cell.x + cell.y + cell.z) % 2 == 0) {
        color = vec3(0.4f);
    }

    vec4 normal = model * vec4(0.f, 1.f, 0.f, 0.f);
    float diffuse = clamp(abs(dot(normalize(eye - f_pos), normal.xyz)) + 0.5, 0.f, 1.f);
    f_color = vec4(ambient + 0.8 * diffuse * color, 1.0);
}
