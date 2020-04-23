#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal; 
layout(location = 2) in vec3 vertexColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

// Output data ; will be interpolated for each fragment.
out vec3 fragmentColor; 
  
void main(){
  // Output position of the vertex, in clip space : MVP * position
  gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
  
  // The color of each vertex will be interpolated
  // to produce the color of each fragment
  fragmentColor = vertexColor;
}