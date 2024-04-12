#version 450 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 1) uniform image2D sampler;

void main()
{
    uvec2 coords = gl_GlobalInvocationID.xy;
    imageStore(sampler, ivec2(coords), vec4(1.0,1.0,1.0,1.0));
}
