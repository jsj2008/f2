#version 330 core

in vec2 uv;

out vec4 f_color;

uniform ivec2 vp;
uniform mat4 inv_proj;

uniform sampler2D depth_tex;

vec3 uv_to_view(vec2 tex_uv, float depth) {
    vec2 clip_xy = tex_uv * 2.f - vec2(1.f);
    float clip_z = depth;
	vec4 view_pos = inv_proj * vec4(clip_xy, clip_z, 1.f);
	return view_pos.xyz / view_pos.w;
}

vec3 get_view_pos(vec2 tex_uv) {
    float depth = texture(depth_tex, tex_uv).x;
    return uv_to_view(tex_uv, depth);
}

void main() {
    float depth = texture(depth_tex, uv).x;
    if (depth > 0.99f)
        discard;

    vec3 eye_pos = uv_to_view(uv, depth);

    vec3 ddx = get_view_pos(uv + vec2(1.f / vp.x, 0)) - eye_pos;
    vec3 ddx_2 = eye_pos - get_view_pos(uv - vec2(1.f / vp.x, 0));
    if (abs(ddx.z) > abs(ddx_2.z))
        ddx = ddx_2;

    vec3 ddy = get_view_pos(uv + vec2(0, 1.f / vp.y)) - eye_pos;
    vec3 ddy_2 = eye_pos - get_view_pos(uv - vec2(0, 1.f / vp.y));
    if (abs(ddy.z) > abs(ddy_2.z))
        ddy = ddy_2;

    vec3 n = cross(ddx, ddy);
    n = normalize(n);

    f_color = vec4(n, 1.f);

    vec3 ldir = normalize(-eye_pos);
    vec3 base_color = vec3(0.15f, 0.65f, 1.f);
    vec3 ambient = vec3(0.2f, 0.2f, 0.4f);
    float diffuse = clamp(dot(n, ldir), 0.f, 1.f);
    f_color = vec4(ambient + diffuse * base_color, 1.0);
    gl_FragDepth = depth;
}