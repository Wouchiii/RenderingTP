#version 410

in vec2 uv;
in vec3 normal;
out vec4 out_color;

uniform sampler2D my_texture;
uniform vec3 light_direction;

void main()
{
    vec3 norm = normalize(normal);
    float intensity = max(-dot(norm, normalize(light_direction)), 0.0);
    vec4 texture_color = texture(my_texture, uv);
    texture_color.rgb *= intensity;
    out_color = texture_color;
}
