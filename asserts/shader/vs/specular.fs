#version 460 core

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
}