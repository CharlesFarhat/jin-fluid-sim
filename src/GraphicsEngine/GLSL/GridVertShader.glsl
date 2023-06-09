#version 330 core
    layout(location = 3) in vec3 aPos;
layout(location = 4) in float vertexDetector;

uniform mat4 u_projView;
out vec4 vertexColor;

void main()
{
    vertexColor = vec4(0.6, 0.6, 0.2, 0.6);

    if(vertexDetector > 0.0f)
    gl_Position = u_projView * vec4(aPos, 1.0);
    else
    gl_Position =  vec4(0.0, 0.0, 0.0, 1.0);

    gl_PointSize = 1.0;
}