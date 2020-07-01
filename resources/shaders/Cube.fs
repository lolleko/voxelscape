#version 330 core

uniform vec3 lightPos; 
uniform vec3 lightColor; 

in vec3 fragmentPos;
in vec3 fragmentNormal;
in vec3 fragmentColor;

out vec3 color;

void main(){
  // ambient
  float ambientStrength = 0.1;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse
  vec3 norm = normalize(fragmentNormal);
  vec3 lightDir = normalize(lightPos - fragmentPos);

  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  vec3 result = (ambient + diffuse) * fragmentColor;

  color = result;
}