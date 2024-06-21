#version 410

in vec2 uv;
out vec4 out_color;
uniform sampler2D my_texture;

void main()
{
    vec4 texture_color = texture(my_texture, uv);
    out_color = texture_color;
}