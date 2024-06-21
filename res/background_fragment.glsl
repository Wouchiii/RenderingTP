#version 410

in vec2 uv;
out vec4 out_color;
uniform sampler2D screen_texture;

void main() {
    vec4 texColor = texture(screen_texture, uv);
    float grayscale = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
    out_color = vec4(grayscale, grayscale, grayscale, 1.0);
}