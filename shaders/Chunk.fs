#version 330 core

in VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
    vec3 tangent;
    vec3 biTangent;
    vec3 material;
    flat uint blockID;
    float lightLevel;
    flat uint vc;
    flat uint vd;
} i;

out vec4 outColor;

uniform vec3 lightDir;
uniform vec3 lightColor; 
uniform vec3 viewPos;

uniform uvec3 worldSize;

uniform sampler2DArray spriteTexture;
uniform sampler3D shadowTexture;

uniform bool enableShadows;
uniform bool enableAO;
uniform bool showAO;
uniform bool showUV;
uniform bool showNormals;
uniform bool showLight;

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

uint normalToFaceIndex(in vec3 n)
{
    if (n.x == 1) {
        return 0u;
    }
    if (n.x == -1) {
        return 1u;
    }
    if (n.y == 1) {
        return 2u;
    }
    if (n.y == -1) {
        return 3u;
    }
    if (n.z == 1) {
        return 4u;
    }
    if (n.z == -1) {
        return 5u;
    }
}

uvec4 uintToUVec4(uint compressed, uint offset) {
    uint k = offset;
    return uvec4((compressed >> k) & 1u, (compressed >> (k + 1u)) & 1u, (compressed >> (k + 2u)) & 1u, (compressed >> (k + 3u)) & 1u);
}

float calcOcc(in vec3 nor )
{
    uint faceIndex = normalToFaceIndex(nor);
	vec2 uv = i.texCoord;
    uvec4 vc = uintToUVec4(i.vc, faceIndex * 4u);
    uvec4 vd = uintToUVec4(i.vd, faceIndex * 4u);

    vec2 st = 1.0 - uv;

    // edges
    vec4 wa = vec4( uv.x, st.x, uv.y, st.y ) * vc; // TODO change back to vc if uvs figured out

    // corners
    vec4 wb = vec4(uv.x*uv.y,
                   st.x*uv.y,
                   st.x*st.y,
                   uv.x*st.y)*vd*(1.0-vc.xzyw)*(1.0-vc.zywx);
    
    //return wa.x + wa.y + wa.z + wa.w + wb.x + wb.y + wb.z + wb.w;
    return (vc.x + vc.y + vc.z + vc.w + vd.x + vd.y + vd.z + vd.w) / 8.0;//wa.x + wa.y + wa.z + wa.w + wb.x + wb.y + wb.z + wb.w;
}

vec3 applyFog(in vec3  rgb,
               in float distance,
               in vec3  rayDir,
               in vec3  sunDir)
{
    float fogAmount = 1.0 - exp(-distance*0.0006);
    float sunAmount = max(dot( rayDir, sunDir ), 0.0);
    vec3  fogColor  = mix(vec3(0.5,0.6,0.7),
                           vec3(1.0,0.9,0.7),
                           pow(sunAmount,8.0) );
    return mix( rgb, fogColor, fogAmount );
}

vec3 fog(vec3 color, vec3 fcolor, float distance, float density){
    float f = exp(-pow(distance*density, 2));
    return mix(fcolor, color, f);
}       

void main() {
    vec3 norm = normalize(i.normal);

    vec3 rayStart = i.worldPosition;

    vec3 viewDir = normalize(i.worldPosition - viewPos);

    vec3 directLightDir = normalize(lightDir);

    float shadowFactor = enableShadows ? raymarch(rayStart, directLightDir) : 1.0;

    float occ = 1.0;

    if (enableAO) {
        occ = calcOcc(norm);
        occ = 1.0 - occ/8.0;
        occ = occ*occ;
        occ = occ*occ;
    }

    float sun = clamp(dot(norm, directLightDir), 0.05, 1.0 );
    float sky = clamp(0.5 + 0.5 * norm.y, 0.0, 1.0);
    float ind = clamp( dot( norm, normalize(directLightDir*vec3(-1.0,0.0,-1.0)) ), 0.0, 1.0 );

    vec3 light  = sun * vec3(1.3,0.70, 0.45) * vec3(1.05) * pow(vec3(shadowFactor), vec3(1.0,1.2,1.5));
    light += i.lightLevel * vec3(1.3, 0.9, 0.3);
    light += sky*vec3(0.229, 0.607, 0.821)*vec3(0.4)*occ;
    light += ind*vec3(0.35,0.23,0.15)*vec3(0.8)*occ;

    // block material
    vec3 tex =  pow(texture(spriteTexture, vec3(i.texCoord, i.blockID)).rgb, vec3(2.2));

    vec3 color = tex * light;

    //color = applyFog(color, length(viewPos - i.worldPosition),  viewDir, directLightDir);
    color = fog(color, vec3(0.5,0.6,0.7), length(viewPos - i.worldPosition), 0.0022);

    // gamma correction
    color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);

    if (showAO) {
        outColor = vec4(occ, occ, occ, 1.0);
    }

    if (showUV) {
        outColor = vec4(i.texCoord, 0, 1.0);
    }

    if (showNormals) {
        outColor = vec4((norm + 1) / 2, 1.0);
    }

    if (showLight) {
        outColor = vec4(i.lightLevel);
    }

    //outColor = texture(spriteTexture, vec3(i.texCoord, 4));

    //outColor = vec4(occ);

    //outColor = vec4(vec3(i.worldPosition.x + worldSizeHalf.x, i.worldPosition.y + worldSizeHalf.y, i.worldPosition.z + worldSizeHalf.z) / worldSize, 1.0);

    //outColor = vec4((i.worldPosition + worldSize / 2u) / worldSize, 1);

    //outColor = vec4(map(i.worldPosition + vec3(0, 24, 0)) / 255, 0, 0, 1);

    //outColor = vec4(map(i.worldPosition + lightDir * 0.1), 0, 0, 1);
}