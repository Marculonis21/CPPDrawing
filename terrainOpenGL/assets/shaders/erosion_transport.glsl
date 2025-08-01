#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 3) uniform image2D waterFlowSampler;
layout(rgba32f, binding = 4) uniform image2D sedimentSampler;

uniform int tTextureSize;
uniform int wTextureSize;

uniform float timeStep;

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    vec4 sedimentTexture = imageLoad(sedimentSampler, ivec2(coords));

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

    vec2 vel = imageLoad(waterTextureSampler, ivec2(coords)).xy;

    vec2 sampleCoord = coords - vel*timeStep;
    vec2 sFract = fract(sampleCoord);
    float fracX = sFract.x;
    float fracY = sFract.y;

    float x0, x1, y0, y1;
    float s0, s1, t0, t1;

    int state = 0;

    if (fracX < 0.5) {
        x0 = floor(sampleCoord.x);
        x1 = x0+1;

        s0 = 1.0-fracX;
        s1 = 1.0 - s0;
    }
    else {
        x0 = ceil(sampleCoord.x);
        x1 = x0-1;

        s0 = fracX;
        s1 = 1.0 - s0;
        state = state | 1;
    }

    if (fracY < 0.5) {
        y0 = floor(sampleCoord.y);
        y1 = y0+1;

        t0 = 1.0-fracY;
        t1 = 1.0 - t0;
    }
    else {
        y0 = ceil(sampleCoord.y);
        y1 = y0-1;

        t0 = fracY;
        t1 = 1.0 - t0;
        state = state | 2;
    }

    float ul = imageLoad(sedimentSampler, ivec2(x0,y0)).w;
    float ur = imageLoad(sedimentSampler, ivec2(x1,y0)).w;
    float dl = imageLoad(sedimentSampler, ivec2(x0,y1)).w;
    float dr = imageLoad(sedimentSampler, ivec2(x1,y1)).w;

    float newSed = 0;
    if (state == 0) {
        newSed = s0 * ( (t0 * ul) + (t1 * dl))
                +s1 * ( (t0 * ur) + (t1 * dr));
    }
    else if (state == 1) {
        newSed = s1 * ( (t0 * ul) + (t1 * dl))
                +s0 * ( (t0 * ur) + (t1 * dr));
    }
    else if (state == 2) {
        newSed = s0 * ( (t1 * ul) + (t0 * dl))
                +s1 * ( (t1 * ur) + (t0 * dr));
    }
    else {
        newSed = s1 * ( (t1 * ul) + (t0 * dl))
                +s0 * ( (t1 * ur) + (t0 * dr));
    }

    /* float newSed = (ul + dl + ur + dr)/4.0; */
    /* float newSed = imageLoad(sedimentSampler, ivec2(x0,y0)).w; */

    imageStore(sedimentSampler, ivec2(coords), vec4(sedimentTexture.rgb,newSed));
}
