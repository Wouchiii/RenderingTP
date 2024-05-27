#version 410

in vec2 uv;
out vec4 out_color;

void main()
{
    out_color = vec4(uv.x, uv.y, 0.0, 1.0);
}