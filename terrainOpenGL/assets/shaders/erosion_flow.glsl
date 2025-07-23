#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D albedoHeightSampler;
layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 3) uniform image2D waterFlowSampler;

uniform int tTextureSize;
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


    const ivec2 L = ivec2(coords+vec2(-1, 0));
    const ivec2 U = ivec2(coords+vec2( 0, 1));
    const ivec2 R = ivec2(coords+vec2( 1, 0));
    const ivec2 D = ivec2(coords+vec2( 0,-1));
    
    float tHeightL = coords.x == 0              ? tHeight : imageLoad(albedoHeightSampler, L).w;
    float tHeightU = coords.y == tTextureSize-1 ? tHeight : imageLoad(albedoHeightSampler, U).w;
    float tHeightR = coords.x == tTextureSize-1 ? tHeight : imageLoad(albedoHeightSampler, R).w;
    float tHeightD = coords.y == 0              ? tHeight : imageLoad(albedoHeightSampler, D).w;

    float wHeightL = coords.x == 0              ? wHeight : imageLoad(waterTextureSampler, L).w;
    float wHeightU = coords.y == wTextureSize-1 ? wHeight : imageLoad(waterTextureSampler, U).w;
    float wHeightR = coords.x == wTextureSize-1 ? wHeight : imageLoad(waterTextureSampler, R).w;
    float wHeightD = coords.y == 0              ? wHeight : imageLoad(waterTextureSampler, D).w;

    float dh_L = tHeight+wHeight - tHeightL-wHeightL;
    float dh_U = tHeight+wHeight - tHeightU-wHeightU;
    float dh_R = tHeight+wHeight - tHeightR-wHeightR;
    float dh_D = tHeight+wHeight - tHeightD-wHeightD;

    float a_L = (G * dh_L) / L_PIPE;
    float a_U = (G * dh_U) / L_PIPE;
    float a_R = (G * dh_R) / L_PIPE;
    float a_D = (G * dh_D) / L_PIPE;

    //float A_PIPE = tHeight+wHeight * 100;
    float A_PIPE = 60;

    // no slip - no water can flow out of the grid
    flow.r = coords.x == 0              ? 0 : max(0, flow.r + timeStep*A_PIPE*a_L);
    flow.g = coords.y == wTextureSize-1 ? 0 : max(0, flow.g + timeStep*A_PIPE*a_U);
    flow.b = coords.x == wTextureSize-1 ? 0 : max(0, flow.b + timeStep*A_PIPE*a_R);
    flow.a = coords.y == 0              ? 0 : max(0, flow.a + timeStep*A_PIPE*a_D);

    // distance of points (1*1 for now)
    float fSum = flow.r+flow.g+flow.b+flow.a;

    // float K = min(1, (wHeight*1*1)/((fSum)*timeStep));
    float K = min(1, (wHeight*L_CELL*L_CELL)/(fSum*timeStep));

    // imageStore(waterTextureSampler, ivec2(coords), vec4(0,0,0, wHeight));
    imageStore(waterFlowSampler, ivec2(coords), K*flow);
}
