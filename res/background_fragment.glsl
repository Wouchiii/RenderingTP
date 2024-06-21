#version 410

in vec2 uv;
out vec4 out_color;
uniform sampler2D screen_texture;
uniform float contrast; // Ajouter une uniform pour contrôler le niveau de contraste

void main() {
    vec4 color = texture(screen_texture, uv);
    color.rgb = ((color.rgb - 0.5) * max(contrast, 0.0)) + 0.5;
    out_color = vec4(color.rgb, 1.0);
}