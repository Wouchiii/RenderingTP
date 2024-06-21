#version 410

in vec2 uv;
out vec4 out_color;
uniform sampler2D screen_texture;
uniform float time;

void main() {
    vec2 uvDistorted = uv + 0.01 * sin(uv.yx * 40.0 + time);
    vec4 color = texture(screen_texture, uvDistorted);
    out_color = color;
}