#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 3) uniform image2D waterFlowSampler;
layout(rgba32f, binding = 4) uniform image2D sedimentSampler;

uniform int tTextureSize;
uniform int wTextureSize;

uniform float timeStep;

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    vec4 sedimentTexture = imageLoad(sedimentSampler, ivec2(coords));

    vec4 waterTexture = imageLoad(waterTextureSampler, ivec2(coords));
    vec2 vel = waterTexture.xy;
    float waterHeight = waterTexture.w;
    if (waterHeight <= 0) {
        return;
    }

    vec2 sampleCoord = coords - vel*timeStep;
    vec2 fract = fract(sampleCoord);

    ivec2 DL = ivec2(floor(sampleCoord));
    ivec2 DR = ivec2(ceil(sampleCoord.x), floor(sampleCoord.y));
    ivec2 UL = ivec2(floor(sampleCoord.x), ceil(sampleCoord.y));
    ivec2 UR = ivec2(ceil(sampleCoord));

    const int wMax = wTextureSize-1;

    float dl = DL.x < 0    || DL.y < 0    ? 0 : imageLoad(sedimentSampler, DL).y;
    float dr = DR.x > wMax || DR.y < 0    ? 0 : imageLoad(sedimentSampler, DR).y;
    float ul = UL.x < 0    || UL.y > wMax ? 0 : imageLoad(sedimentSampler, UL).y;
    float ur = UR.x > wMax || UR.y > wMax ? 0 : imageLoad(sedimentSampler, UR).y;

    float lower = mix(dl, dr, fract.x);
    float upper = mix(ul, ur, fract.x);

    float newSed = mix(lower, upper, fract.y);

    imageStore(sedimentSampler, ivec2(coords), vec4(newSed, sedimentTexture.yzw));
}
