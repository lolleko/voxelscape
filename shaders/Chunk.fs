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

uniform uvec3 worldSize;

uniform usampler3D shadowTexture;

vec3 worldSizeHalf = worldSize / 2u;

float sdBox(vec3 p)
{
    vec3 d = abs(p) - vec3(1.0);
    return min(max(d.x, max(d.y, d.z)), 0.0) +
        length(max(d, 0.0));
}

float map(vec3 pos) {
    //sdBox(fract(pos)) + 
    //return texture(shadowTexture, (pos + worldSizeHalf) / worldSize).r;
    return texelFetch(shadowTexture, ivec3(pos + worldSizeHalf), 0).r;
}

float raymarch(vec3 ro, vec3 rd) {
    const float maxDist = 256;
    float res = 1.0;
    for (float t = 0.01; t < maxDist;) {
        vec3 p = ro + rd * t; // World space position of sample
        float d = map(p);       // Sample of distance field (see map())

        // If the sample <= 0, we have hit something (see map()).
        if (d == 0.0) {
            return 0.0;
        }

        res = min(res, 4.0*d/t);
        // If the sample > 0, we haven't hit anything yet so we should march forward
        // We step forward by distance d, because d is the minimum distance possible to intersect
        // an object (see map()).
        t += (d * d) / 64.0;
    }
    return 1.0;
    //return clamp(res, 0.0, 1.0);
}

void main() {
    vec3 rayStart = i.worldPosition;
    float shadowFactor = raymarch(rayStart, normalize(lightPos - i.worldPosition));

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightDir = normalize(lightPos - i.worldPosition);
    vec3 norm = normalize(i.normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize((viewPos - i.worldPosition));
    vec3 reflectDir = reflect(-lightDir, norm); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * i.color;

    outColor = vec4(result, 1.0) * shadowFactor;

    //outColor = vec4((i.worldPosition + worldSize / 2u) / worldSize, 1);    

    //outColor = vec4(map(i.worldPosition), 0, 0, 1);
}