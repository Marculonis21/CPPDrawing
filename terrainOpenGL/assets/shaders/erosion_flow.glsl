#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D albedoHeightSampler;
layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 3) uniform image2D waterFlowSampler;

uniform int wTextureSize;

uniform float timeStep;

const float L_CELL = 1;
const float L_PIPE = 1; 
const float G = 9.81;

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;
    float tHeight = imageLoad(albedoHeightSampler, ivec2(coords)).w;
    float wHeight = imageLoad(waterTextureSampler, ivec2(coords)).w;

    vec4 flow = imageLoad(waterFlowSampler, ivec2(coords));
    //    g
    //    ^ 
    // r < > b
    //    v
    //    a
    //    TODO: POSSIBLE EXTENSION TO 8 neighbors
    
    float tHeightL = imageLoad(albedoHeightSampler, ivec2(coords+vec2(-1, 0))).w;
    float tHeightU = imageLoad(albedoHeightSampler, ivec2(coords+vec2( 0, 1))).w;
    float tHeightR = imageLoad(albedoHeightSampler, ivec2(coords+vec2( 1, 0))).w;
    float tHeightD = imageLoad(albedoHeightSampler, ivec2(coords+vec2( 0,-1))).w;

    float wHeightL = imageLoad(waterTextureSampler, ivec2(coords)+ivec2(-1, 0)).w;
    float wHeightU = imageLoad(waterTextureSampler, ivec2(coords)+ivec2( 0, 1)).w;
    float wHeightR = imageLoad(waterTextureSampler, ivec2(coords)+ivec2( 1, 0)).w;
    float wHeightD = imageLoad(waterTextureSampler, ivec2(coords)+ivec2( 0,-1)).w;

    float dh_L = tHeight+wHeight - tHeightL-wHeightL;
    float dh_U = tHeight+wHeight - tHeightU-wHeightU;
    float dh_R = tHeight+wHeight - tHeightR-wHeightR;
    float dh_D = tHeight+wHeight - tHeightD-wHeightD;

    float a_L = (G * dh_L) / L_PIPE;
    float a_U = (G * dh_L) / L_PIPE;
    float a_R = (G * dh_L) / L_PIPE;
    float a_D = (G * dh_L) / L_PIPE;

    const float A_PIPE = wHeight;
    flow.r = max(0, flow.r + timeStep*A_PIPE*a_L);
    flow.g = max(0, flow.g + timeStep*A_PIPE*a_U);
    flow.b = max(0, flow.b + timeStep*A_PIPE*a_R);
    flow.a = max(0, flow.a + timeStep*A_PIPE*a_D);

    // distance of points (1*1 for now)
    float fSum = flow.r+flow.g+flow.b+flow.a;

    // float K = min(1, (wHeight*1*1)/((fSum)*timeStep));
    float K = min(1, (wHeight*L_CELL*L_CELL)/(fSum*timeStep));

    // no slip - no water can flow out of the grid
    if(coords.x == 0) flow.r = 0;
    if(coords.x == wTextureSize-1) flow.b = 0;

    if(coords.y == 0) flow.g = 0;
    if(coords.y == wTextureSize-1) flow.a = 0;

    // imageStore(waterTextureSampler, ivec2(coords), vec4(0,0,0, wHeight));
    imageStore(waterFlowSampler, ivec2(coords), K*flow);
}
