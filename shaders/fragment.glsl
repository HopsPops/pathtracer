#version 330 core

in vec2 texcoords;
in vec3 normals;
in vec3 fragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 ambientColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform vec4 colorOverride;

uniform bool textures;
uniform bool light;

uniform bool ambientLight;
uniform bool specularLight;
uniform bool diffuseLight;


out vec4 color;

void main() {
	vec3 norm = normalize(normals);
	vec3 lightDir = normalize(lightPosition - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	
	float ambientStrength = 0.65f * int(ambientLight);
	float specularStrength = 0.65f * int(specularLight);
	
    vec3 ambient = ambientStrength * ambientColor;
	vec3 diffuse = diff * ambientColor * int(diffuseLight);
	
	vec3 viewDir = normalize(cameraPosition - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
	vec3 specular = specularStrength * spec * ambientColor;

	vec3 lightSum = ambient + diffuse + specular;
	if(!ambientLight && !specularLight && !diffuseLight) {
		lightSum = vec3(1.0f, 1.0f, 1.0f);
	}
	
	vec3 result;
	if(textures) {
		result = lightSum * texture(texture_diffuse1, texcoords).xyz;
	} else {
		result = lightSum;
	}
    result *= colorOverride.xyz;

    color = vec4(result, 1.0f);
}