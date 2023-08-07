#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

#define TIME_STEP 0.01f

void main()
{
    uint id = gl_GlobalInvocationID.x;
    objectData[id].a_ip1 = clamp(objectData[id].a_ip1, -100.0f, 100.0f);

    objectData[id].x_ip1 = objectData[id].x_i + objectData[id].v_i*TIME_STEP + (1.0f/2.0f)*objectData[id].a_i*pow(TIME_STEP,2.0f);
    objectData[id].v_ip1 = objectData[id].v_i + (1.0f/2.0f)*(objectData[id].a_i+objectData[id].a_ip1)*TIME_STEP;

    objectData[id].x_i = vec4(objectData[id].x_ip1);
    objectData[id].v_i = vec4(objectData[id].v_ip1);
    objectData[id].a_i = objectData[id].a_ip1;
    objectData[id].a_ip1 = vec4(0.0f,0.0f,0.0f,0.0f);

    vec4 color = vec4(1,1,1,1);
    imageStore(screen, ivec2(objectData[id].x_i), color);
}
