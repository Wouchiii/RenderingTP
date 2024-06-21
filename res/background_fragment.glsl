#version 410

in vec2 uv;
out vec4 out_color;
uniform sampler2D screen_texture;

void main() {
    vec4 texColor = texture(screen_texture, uv);
    out_color = vec4(texColor.r, 0.0, 0.0, 1.0);
}