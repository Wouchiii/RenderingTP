#version 410

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;

uniform mat4 view_projection_matrix;

out vec2 uv;

void main()
{ 
    uv = in_uv;
    gl_Position = view_projection_matrix * vec4(in_position, 0., 1.);
}