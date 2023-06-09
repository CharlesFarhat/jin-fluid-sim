#version 330 core
    layout(location = 2) in vec3 aPos;

uniform mat4 u_projView;
out vec4 vertexColor;

void main()
{
    vertexColor = vec4(1.0, 1.0, 1.0, 1.0);
    gl_Position = u_projView * vec4(aPos, 1.0);
    gl_PointSize = 4.0;
}