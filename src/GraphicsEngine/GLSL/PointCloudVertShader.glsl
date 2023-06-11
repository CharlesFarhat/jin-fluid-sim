#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aCol;

uniform int u_pointSize;
uniform mat4 u_projView;

out vec4 vertexPos;
out vec4 vertexCol;

void main()
{
    vertexPos = vec4(aPos.xyz, 1.0);
    gl_Position = u_projView * vertexPos;

    vec4 eye = u_projView * vertexPos;
    float d = length(eye);
    gl_PointSize = u_pointSize * max(8.0 * 1.0/(0.04 + 0.8*d + 0.0002*d*d), 0.5);

    vertexCol = aCol;
}
