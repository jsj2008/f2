#version 330 core

in vec2 uv;

out vec4 f_color;

uniform ivec2 vp;
uniform mat4 inv_proj;

uniform sampler2D screen_tex;
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
    vec4 color = texture(screen_tex, uv);
    if (color.a != 1.f)
        discard;

    float depth = texture(depth_tex, uv).x;
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

    vec3 l = normalize(-eye_pos);
    float diffuse = 0.5f + 0.5f * dot(n, l);
    vec4 out_color = vec4(diffuse, diffuse, diffuse, 1.f);

    f_color = out_color;
}