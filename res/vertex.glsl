#version 410

layout(location = 0) in vec2 in_position;
uniform mat4 view_projection_matrix;

void main()
{ 
    gl_Position = view_projection_matrix * vec4(in_position, 0., 1.);
}