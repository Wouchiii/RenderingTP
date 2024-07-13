#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

uniform mat4 view_projection_matrix;
uniform mat4 model_matrix;

out vec2 uv;
out vec3 normal;
out vec3 fragPosition;

void main()
{
    uv = in_uv;
    normal = mat3(transpose(inverse(model_matrix))) * in_normal;
    fragPosition = vec3(model_matrix * vec4(in_position, 1.0));
    gl_Position = view_projection_matrix * model_matrix * vec4(in_position, 1.);
}
