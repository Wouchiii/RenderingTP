#version 410

in vec2 uv;
out vec4 out_color;
uniform sampler2D screen_texture;

void main() {
    out_color = texture(screen_texture, uv);
}