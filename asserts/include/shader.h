#pragma once

//vs
const char* const AMBIENT_VS = R"(#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
//uniform mat4 NM;

//out vec3 outNormal;

void main()
{
	gl_Position = P * V * M * vec4(inPos, 1.0);
})";
const char* const DIFFUSE_VS = R"(#version 460 core

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
})";
const char* const SPECULAR_VS = R"(#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NM;

out vec3 normal;
out vec3 viewPos;

void main()
{
	normal =  mat3(NM) * inNormal;
	viewPos = (M * vec4(inPos,1.0)).xyz;
	gl_Position = P * V * M * vec4(inPos, 1.0);
})";

//fs
const char* const AMBIENT_FS = R"(#version 460 core

uniform float ambientStrength;
uniform vec4 lightColor;
uniform vec4 objColor;

out vec4 FragColor;

void main()
{
	vec4 ambientLightColor = ambientStrength * lightColor;
	vec4 ambientColor = ambientLightColor * objColor;
	FragColor = ambientColor;
})";
const char* const DIFFUSE_FS = R"(#version 460 core

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
})";
const char* const SPECULAR_FS = R"(#version 460 core

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float shiness;
uniform float ambientStrength;
uniform vec4 lightColor;
uniform vec4 objColor;

in vec3 normal;
in vec3 viewPos;

out vec4 FragColor;

void main()
{
	vec4 ambientLightColor = ambientStrength * lightColor;
	float diffuseStrength = 0.0;
	float specularStrength = 0.0;

	//diffuse
	vec3 lightDirection = normalize(lightPos);
	vec3 normalDirection = normalize(normal);
	diffuseStrength = clamp(dot(lightDirection,normalDirection),0.0,1.0) * 0.8;
	vec4 diffuseColor = diffuseStrength * lightColor;

	//specular
	//float flag = step(0.0, dot(-lightDirection, normalDirection)); //>0.0,retrn 1.0; <=0.0,return 0;fix specular on back face
	if(diffuseStrength == 0.0)
	{
		specularStrength = 0.0;
	}
	else
	{
		vec3 reflectDirection = normalize(reflect(-lightDirection, normalDirection));
		vec3 viewDirection = normalize(cameraPos - viewPos);
		specularStrength = pow(clamp(dot(reflectDirection,viewDirection), 0.0, 1.0), shiness);
	}


	vec4 specularColor = specularStrength * lightColor;

	FragColor = (ambientLightColor + diffuseColor + specularColor) * objColor;
})";
