#version 430 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

struct gravitationalObject
{
    vec4 x_i; 
    vec4 x_ip1; 
    vec4 v_i; 
    vec4 v_ip1; 
    vec4 a_i; 
    vec4 a_ip1; 
};

layout(std430, binding = 2) buffer gravitationalObjectData
{
     gravitationalObject objectData[];
};

#define G 10000f

void main()
{
    uint current = gl_GlobalInvocationID.x;
    uint other   = gl_GlobalInvocationID.y;

    if (current == other) return;

    vec4 r_ij = objectData[other].x_i - objectData[current].x_i;

    objectData[current].a_ip1 += (G*r_ij)/pow(length(r_ij),3.0);
}
