#version 330 core

in VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
    vec3 tangent;
    vec3 biTangent;
    vec3 color;
} i;

out vec4 outColor;

uniform vec3 lightPos; 
uniform vec3 lightColor; 
uniform vec3 viewPos;

uniform sampler2D texture_normal1;

void main(){
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
	vec3 norm = normalize(texture(texture_normal1, i.texCoord).rgb * 2.0 - 1.0);

    mat3 TBN = mat3(
		normalize(i.tangent),
		normalize(i.biTangent),
		normalize(i.normal)	
	);
    // Transform normal from tangent to world space
    norm = TBN * norm;

    // perform light calculations in world space
    vec3 lightDir = normalize((lightPos - i.worldPosition));

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize((viewPos - i.worldPosition));
    vec3 reflectDir = reflect(-lightDir, norm); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * i.color;

    outColor = vec4(result, 1.0);
}