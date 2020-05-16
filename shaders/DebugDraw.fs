#version 330 core

in VertexData {
    vec3 color;
} i;

out vec3 color;

void main(){
    color = i.color;
}