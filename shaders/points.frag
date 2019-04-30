#version 330 core

flat in vec4 f_pos;

out vec4 f_color;

uniform float radius;
uniform mat4 view;
uniform mat4 proj;

void main() {
    vec3 n;
    n.xy = 2 * gl_PointCoord.st - vec2(1.f);
    n.y = -n.y;
    float r2 = dot(n.xy, n.xy);
    if (r2 > 1.f)
        discard;
    n.z = sqrt(1.f - r2);

	vec3 view_space_pos = (view * f_pos).xyz;
    vec4 pos = vec4(view_space_pos + n * radius, 1.f);
    vec4 clip_pos = proj * pos;
    float ndc_depth = clip_pos.z / clip_pos.w;
    gl_FragDepth = (gl_DepthRange.diff * ndc_depth + gl_DepthRange.near + gl_DepthRange.far) / 2.f;
    f_color = vec4(gl_FragDepth);

    vec3 ldir = normalize(-pos.xyz);
    vec3 color = vec3(0.15f, 0.65f, 1.f);
    vec3 ambient = vec3(0.2f, 0.2f, 0.4f);
    float diffuse = clamp(dot(n, ldir), 0.f, 1.f);
    f_color = vec4(ambient + diffuse * color, 1.0);
}
