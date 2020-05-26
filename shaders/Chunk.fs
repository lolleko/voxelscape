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
    // WIP
    vec3 absPos = abs(pos - ivec3(pos) + 0.5);
    vec3 d = absPos - vec3(1.0);

    ivec3 shadowTexCoord = clamp(ivec3(pos + worldSizeHalf), ivec3(0.0), ivec3(worldSize) - ivec3(1.0));

    // TODO for dist = 1 find actual closest surface
    return float(texelFetch(shadowTexture, shadowTexCoord, 0).r) -1.0 - min(sdBox(pos - ivec3(pos) + 0.5), -0.999);
    //return sdBox(fract(pos)) + float(texelFetch(shadowTexture, min(ivec3(pos + worldSizeHalf), ivec3(worldSize) - ivec3(1.0)), 0).r) + 1.0;
}

float raymarch(vec3 ro, vec3 rd) {
    float res = 1.0;
    float ph = 1e20;

    const float mint = 0.1;
    const float maxt = 255.0;

    const float k = 8.0;

    for( float t = mint; t<maxt; )
    {
        float h = map(ro + rd * t);

        float y = h*h/(2.0*ph);
        float d = sqrt(h*h-y*y);
        res = min( res, k*d/max(0.0,t-y) );
        ph = h;
        t += h;

        if( h <= 0.001 ) {
            break;
        }
    }
    //return 1.0;
    return res;
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
    vec3 viewDir = normalize(viewPos - i.worldPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * i.color * shadowFactor;

    outColor = vec4(result, 1.0);

    //outColor = vec4(vec3(i.worldPosition.x + worldSizeHalf.x, i.worldPosition.y + worldSizeHalf.y, i.worldPosition.z + worldSizeHalf.z) / worldSize, 1.0);

    //outColor = vec4((i.worldPosition + worldSize / 2u) / worldSize, 1);

    //outColor = vec4(map(i.worldPosition + vec3(0, 24, 0)) / 255, 0, 0, 1);
}