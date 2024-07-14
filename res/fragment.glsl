#version 410

in vec2 uv;
in vec3 normal;
in vec3 fragPosition;
out vec4 out_color;

uniform sampler2D my_texture;
uniform vec3 light_position;
uniform vec3 light_intensity;
uniform vec3 light_direction;
uniform float directional_light_intensity;
uniform float ambient_strength;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light_direction);
    float diff = max(dot(norm, lightDir), 0.0) * directional_light_intensity;

    vec3 pointLightDir = normalize(light_position - fragPosition);
    float distance = length(light_position - fragPosition);
    float pointDiff = max(dot(norm, pointLightDir), 0.0) / (distance * distance);

    vec3 ambientColor = vec3(0.0, 0.0, 1.0);  // Bleu pour la lumière ambiante
    vec3 pointLightColor = vec3(1.0, 0.0, 0.0);  // Rouge pour la lumière ponctuelle

    vec3 pointLightEffect = pointDiff * light_intensity * pointLightColor;
    vec3 ambientEffect = ambient_strength * ambientColor;

    // Combinaison des contributions lumineuses
    vec3 light = ambientEffect + diff * pointLightColor + pointLightEffect;  // Utilisation de la somme des contributions lumineuses

    vec4 texture_color = texture(my_texture, uv);
    vec3 finalColor = light * texture_color.rgb;  // Multiplier par la couleur de la texture
    out_color = vec4(finalColor, texture_color.a);
}
