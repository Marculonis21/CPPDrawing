#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;
layout(rgba32f, binding = 1) uniform image2D map;

struct soundAgent
{
    vec4 position;
    vec4 dir;
    float lifeCycle;
};

layout(std430, binding = 2) buffer soundAgentData
{
    soundAgent agentData[];
};

#define FREE_COLOR vec4(0,0,0,1.0f)
#define SOUND_COLOR vec4(1,0,0,0.0f)
#define LIFE_FALLOFF 0.5f

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if (agentData[id].lifeCycle < 0)
        return;


    ivec2 coords = ivec2(agentData[id].position);

    vec4 color = vec4(agentData[id].lifeCycle/100.0f);

    imageStore(screen, ivec2(coords), color);

    vec4 nextPos1 = agentData[id].position + agentData[id].dir*1;
    vec4 nextPos2 = agentData[id].position + agentData[id].dir*2;
    if(imageLoad(map, ivec2(nextPos2)) == FREE_COLOR)
    {
        agentData[id].position = nextPos2;
        agentData[id].lifeCycle -= LIFE_FALLOFF;
    }
    else if(imageLoad(map, ivec2(nextPos1)) == FREE_COLOR)
    {
        agentData[id].position = nextPos1;
        agentData[id].lifeCycle -= LIFE_FALLOFF;
    }
}
