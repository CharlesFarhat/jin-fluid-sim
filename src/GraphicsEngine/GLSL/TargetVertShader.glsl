#version 330 core
layout(location = 5) in vec3 aPos;

uniform mat4 u_projView;
out vec4 vertexColor;

void main()
{
    vertexColor = vec4(1.0, 1.0, 0.0, 1.0);
    gl_Position = u_projView * vec4(aPos, 1.0);

    vec4 eye = u_projView * vec4(aPos, 1.0);
    float d = length(eye);
    gl_PointSize = 5 * max(8.0 * 1.0/(0.04 + 0.8*d + 0.0002*d*d), 0.8);
}