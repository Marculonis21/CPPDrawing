#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 3) uniform image2D waterFlowSampler;
layout(rgba32f, binding = 4) uniform image2D sedimentSampler;

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

    vec2 veld = vec2(in_flowL - outFlow.r + outFlow.b - in_flowR,
                     in_flowD - outFlow.a + outFlow.g - in_flowU)/2;
    
    vec2 vel = veld / max(imageLoad(waterTextureSampler, ivec2(coords)).w, 0.0001);

    vec2 sampleCoord = coords - vel*timeStep;
    float fracX = fract(sampleCoord.x);
    float fracY = fract(sampleCoord.y);

    float x0, x1, y0, y1;
    float s0, s1, t0, t1;

    if (fracX < 0.5) {
        x0 = floor(sampleCoord.x);
        x1 = x0+1;

        s0 = 1-(0.5 - fracX);
        s1 = 1.0 - s0;
    }
    else {
        x0 = ceil(sampleCoord.x);
        x1 = x0-1;

        s0 = 1-(fracX - 0.5);
        s1 = 1.0 - s0;
    }

    if (fracY < 0.5) {
        y0 = floor(sampleCoord.y);
        y1 = y0+1;

        t0 = 1-(0.5 - fracY);
        t1 = 1.0 - t0;
    }
    else {
        y0 = ceil(sampleCoord.y);
        y1 = y0-1;

        t0 = 1-(fracY - 0.5);
        t1 = 1.0 - t0;
    }

    float ul = imageLoad(sedimentSampler, ivec2(x0,y0)).w;
    float ur = imageLoad(sedimentSampler, ivec2(x1,y0)).w;
    float dl = imageLoad(sedimentSampler, ivec2(x0,y1)).w;
    float dr = imageLoad(sedimentSampler, ivec2(x1,y1)).w;

    /* float newSed = s0 * ( (t0 * ul) + (t1 * dl)) */
    /*               +s1 * ( (t0 * ur) + (t1 * dr)); */
    float newSed = (ul + dl + ur + dr)/4.0;

    imageStore(sedimentSampler, ivec2(coords), vec4(0,0,0,newSed));
}
