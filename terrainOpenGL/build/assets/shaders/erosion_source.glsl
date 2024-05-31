#version 450 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;

uniform float timeStep;

const float waterAddAmount = 2;

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    float waterAmount = imageLoad(waterTextureSampler, ivec2(coords)).w;
    waterAmount += timeStep*waterAddAmount;

    imageStore(waterTextureSampler, ivec2(coords), vec4(0,0,0,waterAmount));
}
