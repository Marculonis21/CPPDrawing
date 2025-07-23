#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 3) uniform image2D waterFlowSampler;

uniform int wTextureSize;
uniform float timeStep;

const float L_CELL = 1;
const float L_PIPE = 1; 
const float G = 9.81;

//    g
//    ^ 
// r < > b
//    v
//    a
void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    const ivec2 L = ivec2(coords+vec2(-1, 0));
    const ivec2 U = ivec2(coords+vec2( 0, 1));
    const ivec2 R = ivec2(coords+vec2( 1, 0));
    const ivec2 D = ivec2(coords+vec2( 0,-1));

    float in_flowL = coords.x == 0              ? 0 : imageLoad(waterFlowSampler, L).b;
    float in_flowU = coords.y == wTextureSize-1 ? 0 : imageLoad(waterFlowSampler, U).a;
    float in_flowR = coords.x == wTextureSize-1 ? 0 : imageLoad(waterFlowSampler, R).r;
    float in_flowD = coords.y == 0              ? 0 : imageLoad(waterFlowSampler, D).g;

    vec4 outFlow = imageLoad(waterFlowSampler, ivec2(coords));
    //    g
    //    ^ 
    // r < > b
    //    v
    //    a

    float fsumIn = in_flowL + in_flowU + in_flowR + in_flowD;
    float fsumOut = outFlow.r + outFlow.g + outFlow.b + outFlow.a;
    float dV = (fsumIn - fsumOut) * timeStep;

    float wHeight = imageLoad(waterTextureSampler, ivec2(coords)).w;
    float wHeightNew = wHeight + dV/(L_CELL*L_CELL);

    vec2 dFlow = vec2(in_flowL - outFlow.r + outFlow.b - in_flowR,
                      in_flowD - outFlow.a + outFlow.g - in_flowU)/2;

    float wHeightAvg = (wHeight + wHeightNew) / 2.0;

    vec2 newVel = dFlow / (L_CELL * wHeightAvg);

    vec4 waterTexture = vec4(newVel, wHeight, wHeightNew);
    //waterTexture.xy = newVel;
    //waterTexture.z = wHeight; // keep old around
    //waterTexture.w = wHeightNew;

    imageStore(waterTextureSampler, ivec2(coords), waterTexture);
}
