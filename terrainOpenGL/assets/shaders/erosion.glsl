#version 450 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    float height = max(cos(coords.x/10.0)*0.05, -0.001);

    /* barrier(); */
    imageStore(waterTextureSampler, ivec2(coords), vec4(height,height,height,1.0));
}
