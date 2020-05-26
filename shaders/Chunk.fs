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

uniform sampler3D shadowTexture;

vec3 worldSizeHalf = worldSize / 2u;

float sdBox(vec3 p)
{
    vec3 d = abs(p) - vec3(1.0);
    return min(max(d.x, max(d.y, d.z)), 0.0) +
        length(max(d, 0.0));
}

float map(vec3 pos) {
    // WIP

    vec3 shadowTexCoord = (pos + worldSizeHalf) / vec3(worldSize - 1u);
    float distance = texture(shadowTexture, shadowTexCoord).r;

    return distance;


    // ivec3 shadowTexCoord = clamp(ivec3(pos + worldSizeHalf), ivec3(0.0), ivec3(worldSize) - ivec3(1.0));
    //float distance = texelFetch(shadowTexture, shadowTexCoord, 0).r);
    //return float(distance -1.0 - min(sdBox(pos - ivec3(pos) + 0.5), -0.999);
}

float raymarch(vec3 ro, vec3 rd) {
    float res = 1.0;
    float ph = 1e30;

    const float mint = 0.001;
    float t = mint;
    const float maxt = 256.0;

    const float k = 4.0;

    for( int i=0; i<64; i++ )
    {
        float h = map(ro + rd * t);

        float y = h*h/(2.0*ph);
        float d = sqrt(h*h-y*y);
        res = min( res, k*d/max(0.0,t-y) );
        ph = h;
        t += h;

        if( res <= 0.1 || t > maxt) {
            break;
        }
    }

    return clamp(res, 0.0, 1.0);
}

void main() {
    vec3 norm = normalize(i.normal);


    vec3 rayStart = i.worldPosition;

    vec3 lightDir = normalize(lightPos - i.worldPosition);

    float shadowFactor = 0.0;

    // dont calculate shadows if light is behind face
    if (dot(norm, lightDir) >= 0) {
       shadowFactor = raymarch(rayStart, lightDir);
    }

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - i.worldPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + shadowFactor * (diffuse + specular)) * i.color;

    outColor = vec4(result, 1.0);

    //outColor = vec4(vec3(i.worldPosition.x + worldSizeHalf.x, i.worldPosition.y + worldSizeHalf.y, i.worldPosition.z + worldSizeHalf.z) / worldSize, 1.0);

    //outColor = vec4((i.worldPosition + worldSize / 2u) / worldSize, 1);

    //outColor = vec4(map(i.worldPosition + vec3(0, 24, 0)) / 255, 0, 0, 1);

    //outColor = vec4(map(i.worldPosition) / 255, 0, 0, 1);
}