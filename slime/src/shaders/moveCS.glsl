#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

struct slimeAgent
{
    vec4 position;
    vec4 dir;
};

layout(std430, binding = 2) buffer slimeAgentData
{
     slimeAgent agentData[];
};

#define SENSOR_LENGTH 50.0f
#define SENSOR_SIZE 1
#define STEER_STRENGHT 0.5f 

#define AGENT_SPEED 1.0f

#define WIDTH 1024
#define HEIGHT 1024

float sense(uint id, float centerOffset)
{
    vec4 agentDir = agentData[id].dir;
    vec3 upDir    = vec3(0,0,1);

    vec3 right = cross(vec3(agentDir), upDir);
    vec4 agentRightDir = normalize(vec4(right.x,right.y,right.z,0));

    vec4 senseDir = normalize(agentDir + centerOffset*agentRightDir);

    ivec2 senseSpot = ivec2(agentData[id].position + senseDir*SENSOR_LENGTH);

    vec4 sum = vec4(0,0,0,0);

    for(int _y = -SENSOR_SIZE; _y <= SENSOR_SIZE; _y++)
    {
        for(int _x = -SENSOR_SIZE; _x <= SENSOR_SIZE; _x++)
        {
            ivec2 pos = senseSpot + ivec2(_x, _y);

            if(pos.x >= 0 && pos.x < WIDTH &&
               pos.y >= 0 && pos.y < HEIGHT)
            {
                sum += imageLoad(screen, pos);
            }
        }
    }

    return sum.x;
}

void steerAgent(uint id)
{
    float forwardValue = sense(id, 0.0f);
    float rightValue   = sense(id, 1.0f);
    float leftValue    = sense(id,-1.0f);

    float steerValue = 0;

    if(forwardValue > rightValue && forwardValue > leftValue)
    {
        steerValue = 0;
    }
    else if (rightValue > leftValue)
    {
        steerValue = 1;
    }
    else if (leftValue > rightValue)
    {
        steerValue = -1;
    }

    vec4 agentDir = agentData[id].dir;
    vec3 upDir    = vec3(0,0,1);

    vec3 right = normalize(cross(vec3(agentDir), upDir));
    vec4 agentRightDir = normalize(vec4(right.x,right.y,right.z,0));

    agentData[id].dir = normalize(agentData[id].dir + agentRightDir*steerValue*STEER_STRENGHT);
}

void reboundAgent(uint id)
{
    vec4 p = agentData[id].position;
    if(p.x < 0 || p.x > WIDTH)
    {
        agentData[id].dir.x *= -1;
    }

    if(p.y < 0 || p.y > HEIGHT)
    {
        agentData[id].dir.y *= -1;
    }
}

void main()
{
    uint id = gl_GlobalInvocationID.x;

    steerAgent(id);

    agentData[id].position += agentData[id].dir*AGENT_SPEED;

    reboundAgent(id);

    imageStore(screen, ivec2(agentData[id].position), vec4(1,1,1,1));
}
