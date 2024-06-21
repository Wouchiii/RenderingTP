#version 410

in vec2 uv;
out vec4 out_color;
uniform sampler2D screen_texture;
uniform float vignetteAmount; // Contrôle l'intensité du vignettage

void main() {
    vec4 color = texture(screen_texture, uv);
    float dist = distance(uv, vec2(0.5, 0.5));
    color.rgb *= smoothstep(0.8, vignetteAmount, dist * (1.0 + vignetteAmount));
    out_color = color;
}