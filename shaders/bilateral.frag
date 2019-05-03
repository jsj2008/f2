#version 330 core

in vec2 uv;

uniform vec2 filter_dir;
uniform int filter_radius;
uniform float blur_scale;
uniform float blur_depth_fall_off;
uniform sampler2D depth_tex;

void main() {
    float depth = texture(depth_tex, uv).x;
    float sum = 0;
    float wsum = 0;

    float s, r, w, r2, g;
    vec2 dp;

    for(int x = -filter_radius; x <= filter_radius; ++x) {
        dp = x * filter_dir;
        s = texture(depth_tex, uv + dp).x;

        // spatial domain
        r = length(dp) * blur_scale;
        w = exp(-r * r);

        // range domain
        r2 = (s - depth) * blur_depth_fall_off;
        g = exp(-r2 * r2);

        sum += s * w * g;
        wsum += w * g;
    }

    if (wsum > 0.0)
        sum /= wsum;

    gl_FragDepth = sum;
}