#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D albedoHeightSampler;
layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 4) uniform image2D sedimentSampler;

uniform float timeStep;
uniform float evaporationConst;

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    vec4 waterTexture = imageLoad(waterTextureSampler, ivec2(coords));
    vec4 sedTexture = imageLoad(sedimentSampler, ivec2(coords));
    vec4 albedoHeight = imageLoad(albedoHeightSampler, ivec2(coords));

    waterTexture.w = waterTexture.w * (1 - evaporationConst*timeStep);
    //float change = sedTexture.w - sedTexture.w * (1 - evaporationConst*timeStep);
    //sedTexture.w += change;
    //albedoHeight.w -= change;

    imageStore(waterTextureSampler, ivec2(coords), waterTexture);
    //imageStore(albedoHeightSampler, ivec2(coords), albedoHeight);
    //imageStore(sedimentSampler, ivec2(coords), sedTexture);
}
