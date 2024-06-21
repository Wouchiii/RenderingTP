#version 410

in vec2 uv;
in vec3 normal;
out vec4 out_color;

uniform sampler2D my_texture;

void main()
{
    vec4 texture_color = texture(my_texture, uv);
    out_color = vec4(normalize(normal), 1.0);
}
