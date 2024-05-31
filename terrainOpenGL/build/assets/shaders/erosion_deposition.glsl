#version 450 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 3) uniform image2D waterFlowSampler;

uniform float timeStep;

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    float in_flowL = imageLoad(waterFlowSampler, ivec2(coords)+ivec2(-1, 0)).b;
    float in_flowU = imageLoad(waterFlowSampler, ivec2(coords)+ivec2( 0, 1)).a;
    float in_flowR = imageLoad(waterFlowSampler, ivec2(coords)+ivec2( 1, 0)).r;
    float in_flowD = imageLoad(waterFlowSampler, ivec2(coords)+ivec2( 0,-1)).g;

    if(coords.x - 1 < 0) in_flowL = 0;
    if(coords.y + 1 > 1023) in_flowU = 0;
    if(coords.x + 1 > 1023) in_flowR = 0;
    if(coords.y - 1 < 0) in_flowD = 0;

    vec4 outFlow = imageLoad(waterFlowSampler, ivec2(coords));
    //    g
    //    ^ 
    // r < > b
    //    v
    //    a

    float fIn = in_flowL + in_flowU + in_flowR + in_flowD;
    float fOut = outFlow.r + outFlow.g + outFlow.b + outFlow.a;

    float vold = (fIn - fOut) * timeStep;

    float wHeight = imageLoad(waterTextureSampler, ivec2(coords)).w;

    wHeight += vold/(1*1);

    imageStore(waterTextureSampler, ivec2(coords), vec4(0,0,0,wHeight));
}
