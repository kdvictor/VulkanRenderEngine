#version 460 core

uniform float ambientStrength;
uniform vec4 lightColor;
uniform vec4 objColor;

out vec4 FragColor;

void main()
{
	vec4 ambientLightColor = ambientStrength * lightColor;
	vec4 ambientColor = ambientLightColor * objColor;
	FragColor = ambientColor;
}