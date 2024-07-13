#version 410

in vec2 uv;
in vec3 normal;
out vec4 out_color;

uniform sampler2D my_texture;
uniform vec3 light_direction;
uniform float ambient_strength = 0.3;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light_direction);
    float diff = max(dot(norm, lightDir), 0.0);
    float intensity = diff + ambient_strength;
    vec4 texture_color = texture(my_texture, uv);
    texture_color.rgb *= intensity;

    out_color = texture_color;
}
