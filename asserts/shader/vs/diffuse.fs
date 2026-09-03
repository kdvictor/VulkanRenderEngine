#version 460 core

uniform vec3 lightPos;
uniform float ambientStrength;
uniform vec4 lightColor;
uniform vec4 objColor;

in vec3 normal;

out vec4 FragColor;

void main()
{
	vec4 ambientLightColor = ambientStrength * lightColor;

	//diffuse
	vec3 lightDirection = normalize(lightPos);
	vec3 normalDirection = normalize(normal);
	float diffuseStrength = clamp(dot(lightDirection,normalDirection),0.0,1.0) * 0.8;
	vec4 diffuseColor = diffuseStrength * lightColor;

	FragColor = (ambientLightColor + diffuseColor) * objColor;
}