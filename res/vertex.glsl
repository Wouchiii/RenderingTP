#version 410

layout(location = 0) in vec2 in_position;
uniform float aspect_ratio;
uniform float time;

void main()
{
    float radius = 1.0;
    float speed = 1.0;
    
    vec2 position = in_position;

    position.x = position.x / aspect_ratio + radius * cos(time * speed) / aspect_ratio;
    position.y += radius * sin(time * speed);

    gl_Position = vec4(position, 0., 1.);
}