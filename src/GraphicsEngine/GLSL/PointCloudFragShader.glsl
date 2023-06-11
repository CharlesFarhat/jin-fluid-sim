#version 330 core
    in vec4 vertexPos;
in vec4 vertexCol;

uniform vec3 u_cameraPos;

out vec4 fragColor;

void main()
{
    // Additive alpha blending
    vec3 xyz = u_cameraPos.xyz - vertexPos.xyz;

    // WIP not working well
    // Alpha tending to 1 close from the camera
    // to see non translucent close neighbor particles
    // And down to 0.75 away from the camera to allow additive blending
    float r2 = dot(xyz, xyz);
    fragColor.a = 1.0;
    //fragColor.a = 2.5* exp(-r2 / 100000)+0.75;
    //fragColor.rgb = vertexCol.rgb * fragColor.a;

    fragColor = vec4(vertexCol.rgb, 1.0);
}
