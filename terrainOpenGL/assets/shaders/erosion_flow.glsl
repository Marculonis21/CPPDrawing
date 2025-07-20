#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D albedoHeightSampler;
layout(rgba32f, binding = 1) uniform image2D normalSampler;
layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 3) uniform image2D waterFlowSampler;

uniform int tTextureSize;
uniform int wTextureSize;
uniform float tScalingF;

uniform float timeStep;

const float l = 100; // pipe length 1/l
const float A = 100;
const float g = 9.81;

vec3 get_normal(vec2 coords, float vHeight, float uHeight, float rHeight)
{
    // amount of quad * tess factor
    const float step = 1;
    vec3 vertex = vec3(coords.x,      tTextureSize*vHeight, coords.y);
    vec3 UP     = vec3(coords.x,      tTextureSize*uHeight, coords.y+step);
    vec3 RIGHT  = vec3(coords.x+step, tTextureSize*rHeight, coords.y);

    return -normalize(cross(RIGHT-vertex,UP-vertex));
}

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
    
    float tHeightL = imageLoad(albedoHeightSampler, ivec2(coords+vec2(-1, 0))).w;
    float tHeightU = imageLoad(albedoHeightSampler, ivec2(coords+vec2( 0, 1))).w;
    float tHeightR = imageLoad(albedoHeightSampler, ivec2(coords+vec2( 1, 0))).w;
    float tHeightD = imageLoad(albedoHeightSampler, ivec2(coords+vec2( 0,-1))).w;

    vec3 tNormal = get_normal(coords, tHeight, tHeightU, tHeightR);
    imageStore(normalSampler, ivec2(coords), vec4(tNormal,0));

    float wHeightL = imageLoad(waterTextureSampler, ivec2(coords)+ivec2(-1, 0)).w;
    float wHeightU = imageLoad(waterTextureSampler, ivec2(coords)+ivec2( 0, 1)).w;
    float wHeightR = imageLoad(waterTextureSampler, ivec2(coords)+ivec2( 1, 0)).w;
    float wHeightD = imageLoad(waterTextureSampler, ivec2(coords)+ivec2( 0,-1)).w;

    float dh_L = tHeight+wHeight - tHeightL-wHeightL;
    float dh_U = tHeight+wHeight - tHeightU-wHeightU;
    float dh_R = tHeight+wHeight - tHeightR-wHeightR;
    float dh_D = tHeight+wHeight - tHeightD-wHeightD;

    flow.r = max(0, flow.r + timeStep*A*(g*dh_L)*l);
    flow.g = max(0, flow.g + timeStep*A*(g*dh_U)*l);
    flow.b = max(0, flow.b + timeStep*A*(g*dh_R)*l);
    flow.a = max(0, flow.a + timeStep*A*(g*dh_D)*l);

    // no slip - can flow out of the grid
    if(coords.x == 0) flow.r = 0;
    if(coords.x == wTextureSize-1) flow.b = 0;

    if(coords.y == 0) flow.g = 0;
    if(coords.y == wTextureSize-1) flow.a = 0;

    // distance of points (1*1 for now)
    float fSum = flow.r+flow.g+flow.b+flow.a;
    if (fSum < 0.00001) {
        fSum = 0.00001;
    }

    // float K = min(1, (wHeight*1*1)/((fSum)*timeStep));
    float K = min(1, (wHeight*1*1)/(fSum*timeStep));

    imageStore(waterTextureSampler, ivec2(coords), vec4(0,0,0, wHeight));
    imageStore(waterFlowSampler, ivec2(coords), K*flow);
}
