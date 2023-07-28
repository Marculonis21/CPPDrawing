#version 430 core

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;
layout(rgba32f, binding = 1) uniform image2D map;

#define EMPTY_COLOR vec4(0,0,0,0.0f)
#define CLEAR_COLOR vec4(0,0,0,1.0f)

struct agent
{
    vec4 position;
    vec4 color;
};

layout(std430, binding = 3) buffer randomName
{
    agent agentData[];
};

bool posFree(ivec2 coords)
{
    return (CLEAR_COLOR == imageLoad(map, coords)) && (EMPTY_COLOR == imageLoad(screen, coords) || CLEAR_COLOR == imageLoad(screen, coords));
}
void moveAgent(uint id, vec4 change)
{
    ivec2 tmp = ivec2(agentData[id].position);

    agentData[id].position += change;

    imageStore(screen, ivec2(agentData[id].position), vec4(agentData[id].color));
    imageStore(screen, tmp, CLEAR_COLOR);
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    /* uvec2 coords = gl_GlobalInvocationID.xy; */

    ivec2 dims = imageSize(screen);

    ivec2 coords = ivec2(agentData[id].position.x, agentData[id].position.y);

    /* if (imageLoad(map, ivec2(coords)) == vec4(1,1,1,1.0f)) */
    /*     imageStore(screen, ivec2(coords), vec4(1,0,0,1.0f)); */
    /* else */
    /*     imageStore(screen, ivec2(coords), vec4(0,0,1,1.0f)); */

    /* else */

    /* imageStore(map, ivec2(coords), imageLoad(map, ivec2(coords))); */

    if(coords.y + 1 < dims.y-50 && posFree(coords+ivec2(0,+1)))
    {
        moveAgent(id, vec4(0,+1,0,0));
    }
    else if (coords.y + 1 < dims.y-50 && posFree(coords+ivec2(1,+1)))
    {
        moveAgent(id, vec4(1,+1,0,0));
    }
    else if (coords.y + 1 < dims.y-50 && posFree(coords+ivec2(-1,+1)))
    {
        moveAgent(id, vec4(-1,+1,0,0));
    }
}
