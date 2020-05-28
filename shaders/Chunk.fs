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

uniform bool enableShadows;
uniform bool enableAO;

vec3 worldSizeHalf = worldSize / 2u;

float map(in vec3 pos) {
    vec3 shadowTexCoord = (pos + worldSizeHalf) / vec3(worldSize);
    float distance = texture(shadowTexture, shadowTexCoord).r;

    return distance;


    // ivec3 shadowTexCoord = clamp(ivec3(pos + worldSizeHalf), ivec3(0.0), ivec3(worldSize) - ivec3(1.0));
    //float distance = texelFetch(shadowTexture, shadowTexCoord, 0).r);
    //return float(distance -1.0 - min(sdBox(pos - ivec3(pos) + 0.5), -0.999);
}

// https://www.shadertoy.com/view/lsKcDD
float raymarch(in vec3 ro, in vec3 rd) {
    float res = 1.0;
    float ph = 1e20;

    const float mint = 0.001;
    float t = mint;
    const float maxt = 512.0;

    const float k = 10.0;

    for( int i=0; i<96; i++ )
    {
        float h = map(ro + rd * t);

        //float y = h*h/(2.0*ph);
        float y = (i==0) ? 0.0 : h*h/(2.0*ph);
        float d = sqrt(h*h-y*y);
        res = min( res, k*d/max(0.0,t-y) );
        ph = h;
        t += h;

        if( h <= 0 || res <= 0.01 || t > maxt) {
            break;
        }
    }
    return clamp(res, 0.0, 1.0);
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

    vec3 viewDir = normalize(viewPos - i.worldPosition);

    // dont calculate light if behind face
    if (dot(norm, viewDir) < 0) {
       outColor = vec4(0.0);
       return;
    }

    vec3 rayStart = i.worldPosition;

    vec3 lightDir = normalize(lightPos - i.worldPosition);

    float shadowFactor = enableShadows ? raymarch(rayStart, lightDir) : 1.0;

    // ambient
    float ambientStrength = 0.2;
    float occ = 1.0; // TODO enableAO ? calcAO(i.worldPosition, norm) : 1.0;
    vec3 ambient = occ * ambientStrength * lightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + shadowFactor * (diffuse + specular)) * i.color;

    outColor = vec4(result, 1.0);

    //outColor = vec4(occ);

    //outColor = vec4(vec3(i.worldPosition.x + worldSizeHalf.x, i.worldPosition.y + worldSizeHalf.y, i.worldPosition.z + worldSizeHalf.z) / worldSize, 1.0);

    //outColor = vec4((i.worldPosition + worldSize / 2u) / worldSize, 1);

    //outColor = vec4(map(i.worldPosition + vec3(0, 24, 0)) / 255, 0, 0, 1);

    //outColor = vec4(map(i.worldPosition) / 255, 0, 0, 1);
}