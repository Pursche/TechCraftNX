#version 430

layout(location=0) uniform mat4 g_matWorldViewProj;

layout(location=0) in vec3 vertexPos;
layout(location=1) in vec2 vertexTexcoord;
layout(location=2) in vec3 vertexNormal;

out vec2 texcoord;
out vec3 normal;

void main()
{
    gl_Position = g_matWorldViewProj * vec4(vertexPos, 1);
    texcoord = vertexTexcoord;
	normal = vertexNormal;
}
