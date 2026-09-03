#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NM;

out vec3 normal;

void main()
{
	normal =  mat3(NM) * inNormal;
	gl_Position = P * V * M * vec4(inPos, 1.0);
}