#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);

    float p =  -TexCoords.y;

    float hardness = 1;

    float p1 = pow(min(1.0f, 1.0f - p), hardness);

    FragColor = vec4(mix(vec3(0.556, 0.552, 0.435), vec3(0.229, 0.607, 0.821), p1), 1.0);
}