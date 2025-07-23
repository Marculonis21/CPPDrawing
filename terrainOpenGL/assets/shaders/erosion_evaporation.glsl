#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;

uniform float timeStep;
uniform float evaporationConst;

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    vec4 waterTexture = imageLoad(waterTextureSampler, ivec2(coords));

    //waterTexture.w = max(0, waterTexture.w * (1 - evaporationConst*timeStep));

    imageStore(waterTextureSampler, ivec2(coords), waterTexture);
}
