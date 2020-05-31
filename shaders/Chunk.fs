#version 330 core

in VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
    vec3 tangent;
    vec3 biTangent;
    vec3 material;
} i;

out vec4 outColor;

uniform vec3 lightDir;
uniform vec3 lightColor; 
uniform vec3 viewPos;

uniform uvec3 worldSize;

uniform sampler3D shadowTexture;

uniform bool enableShadows;
uniform bool enableAO;

vec3 worldSizeHalf = worldSize / 2u;

float map(in vec3 pos) {
    vec3 shadowTexCoord = (pos + worldSizeHalf) / vec3(worldSize);
    float distance = texture(shadowTexture, shadowTexCoord).r;
    //float distance = texelFetch(shadowTexture, clamp(ivec3(pos + worldSizeHalf), ivec3(0.0), ivec3(worldSize) - ivec3(1.0)), 0).r;
    return distance + 0.5;


    // ivec3 shadowTexCoord = clamp(ivec3(pos + worldSizeHalf), ivec3(0.0), ivec3(worldSize) - ivec3(1.0));
    //float distance = texelFetch(shadowTexture, shadowTexCoord, 0).r);
    //return float(distance -1.0 - min(sdBox(pos - ivec3(pos) + 0.5), -0.999);
}

// https://www.shadertoy.com/view/lsKcDD
float raymarch(in vec3 ro, in vec3 rd) {
    float res = 1.0;

    const int maxSteps = 48;

    const float mint = 0.001;
    float t = mint;
    const float maxt = 256.0;

    // lower values => softer
    const float softness = 8.0;

    for(int i=0; i < maxSteps; i++)
    {
        float h = map(ro + rd * t);
        float s = clamp(softness*h/t,0.0,1.0);
        res = min(res, s*s*(3.0-2.0*s));

        t += h;

        if(res <= 0.01 || t > maxt) {
            break;
        }
    }
    return clamp(res, 0.2, 1.0);
}

float calcAO( in vec3 pos, in vec3 nor )
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float h = 0.3 + 0.3*float(i);
        float d = map( pos + h*nor );
        occ += (h-d)*sca;
        sca *= 0.5;
    }
    return clamp( 1.0 - 1.5 * occ, 0.0, 1.0 );    
}

void main() {
    vec3 norm = normalize(i.normal);

    vec3 rayStart = i.worldPosition;

    vec3 directLightDir = normalize(lightDir);

    float shadowFactor = enableShadows ? raymarch(rayStart, directLightDir) : 1.0;

    float sun = clamp(dot(norm, directLightDir), 0.0, 1.0 );

    vec3 lin  = sun * vec3(1.00,0.80,0.55) * pow(vec3(shadowFactor), vec3(1.0,1.2,1.5));
    // TODO sky light and indirect light

    // block material
    vec3 color = i.material * lin;

    // gamma correction
    color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);

    //outColor = vec4(occ);

    //outColor = vec4(vec3(i.worldPosition.x + worldSizeHalf.x, i.worldPosition.y + worldSizeHalf.y, i.worldPosition.z + worldSizeHalf.z) / worldSize, 1.0);

    //outColor = vec4((i.worldPosition + worldSize / 2u) / worldSize, 1);

    //outColor = vec4(map(i.worldPosition + vec3(0, 24, 0)) / 255, 0, 0, 1);

    //outColor = vec4(map(i.worldPosition + lightDir * 0.1), 0, 0, 1);
}