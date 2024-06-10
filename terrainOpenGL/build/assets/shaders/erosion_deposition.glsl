#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D albedoHeightSampler;
layout(rgba32f, binding = 1) uniform image2D normalSampler;
layout(rgba32f, binding = 2) uniform image2D waterTextureSampler;
layout(rgba32f, binding = 3) uniform image2D waterFlowSampler;
layout(rgba32f, binding = 4) uniform image2D sedimentSampler;

uniform int tTextureSize;
uniform int wTextureSize;
uniform float tScalingF;

uniform float timeStep;

uniform float sedCapacityConst;
uniform float sedDissolveConst;
uniform float sedDepositConst;

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    float in_flowL = imageLoad(waterFlowSampler, ivec2(coords)+ivec2(-1, 0)).b;
    float in_flowU = imageLoad(waterFlowSampler, ivec2(coords)+ivec2( 0, 1)).a;
    float in_flowR = imageLoad(waterFlowSampler, ivec2(coords)+ivec2( 1, 0)).r;
    float in_flowD = imageLoad(waterFlowSampler, ivec2(coords)+ivec2( 0,-1)).g;

    if(coords.x - 1 < 0) in_flowL = 0;
    if(coords.y + 1 > wTextureSize-1) in_flowU = 0;
    if(coords.x + 1 > wTextureSize-1) in_flowR = 0;
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

    vec4 waterTexture = imageLoad(waterTextureSampler, ivec2(coords));

    float oldW = waterTexture.w;
    waterTexture.w += vold/(1*1);
    float newW = waterTexture.w;

    vec2 vel = vec2(in_flowL - outFlow.r + outFlow.b - in_flowR,
                    in_flowD - outFlow.a + outFlow.g - in_flowU)/2;

    vec2 velComps = vel/max(((oldW+newW)/2.0),0.001);
    waterTexture.xy = velComps;

    imageStore(waterTextureSampler, ivec2(coords), waterTexture);


    vec4 albedoHeight = imageLoad(albedoHeightSampler, ivec2(coords));
    float tHeight = albedoHeight.w;

    vec3 normal = imageLoad(normalSampler, ivec2(coords)).rgb;
    /* float tilt = acos(dot(normal, vec3(0,1,0))); */
    /* tilt = max(tilt, 0.001); */
    float dotV = dot(normal, vec3(0,1,0));
    float tilt = sqrt(1 - dotV*dotV);
    /* tilt = max(tilt, 0.001); */

    float transportCap = sedCapacityConst * 1 * length(vel);

    float sedAmount = imageLoad(sedimentSampler, ivec2(coords)).w;

    if(transportCap > sedAmount) {
        float change = sedDissolveConst*(transportCap - sedAmount)*timeStep;
        tHeight = tHeight - change;
        sedAmount = sedAmount + change;
    }
    else {
        float change = sedDepositConst*(sedAmount - transportCap)*timeStep;
        tHeight = tHeight + change;
        sedAmount = sedAmount - change;
    }

    imageStore(albedoHeightSampler, ivec2(coords), vec4(albedoHeight.rgb, tHeight));
    imageStore(sedimentSampler, ivec2(coords), vec4(0,0,0,sedAmount));
}
