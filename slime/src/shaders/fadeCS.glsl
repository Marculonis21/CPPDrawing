#version 430 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

#define FALLOFF 0.98f

void main()
{
    uvec2 coords = gl_GlobalInvocationID.xy;

    vec4 base_color = imageLoad(screen, ivec2(coords));

    imageStore(screen, ivec2(coords), base_color*FALLOFF);
}
