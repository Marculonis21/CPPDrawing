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

uniform float K_C; // sediment transport - scaling constant
uniform float ALPHA_MIN; // min local tilt
uniform float K_DMAX; // min local tilt
uniform float K_S; // sed dissolve const
uniform float K_D; // sed deposit const
uniform float K_H; // sed deposit const
uniform float K_H_MIN; // sed deposit const

// ramp function l_MAX
float l_max(float val) {
    return (val >= K_DMAX) ? 1.0 : (1.0 - (K_DMAX - val)/K_DMAX);
}

float local_tilt(vec2 coords) {
    vec3 tNormal = imageLoad(normalSampler, ivec2(coords)).rgb;
    //return acos(dot(tNormal, vec3(0,1,0))); // local tilt
    return dot(tNormal, vec3(0,1,0)); // local tilt
}

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    // load in all textures

    vec4 albedoHeight = imageLoad(albedoHeightSampler, ivec2(coords));
    vec4 waterTexture = imageLoad(waterTextureSampler, ivec2(coords));
    vec4 sedimentTexture = imageLoad(sedimentSampler, ivec2(coords));

    float tHeight = albedoHeight.w;

    vec2 wVel = waterTexture.xy;
    float wHeightOld = waterTexture.z;
    float wHeight = waterTexture.w;
    if(wHeight < 0.0001) return;

    float sedAmount = sedimentTexture.w;
    float hardnesCoef = sedimentTexture.x;

    // find transport cap

    float alpha = local_tilt(coords);

    //float transportCap = K_C * sin(max(ALPHA_MIN, alpha)) * length(wVel) * l_max(wHeightOld); 
    float transportCap = K_C * alpha * length(wVel) * l_max(wHeightOld); 

    // solve dissolve/deposit
    float sedAmountOld = sedAmount;

    if(transportCap > sedAmount) {
        float diff = (transportCap - sedAmount);
        float dissolvedSoil = K_S*hardnesCoef*diff*timeStep;
        //float dissolvedSoil = K_S*diff*timeStep;

        tHeight = tHeight - dissolvedSoil;
        wHeight = wHeight + dissolvedSoil;
        sedAmount = sedAmount + dissolvedSoil;
    }
    else {
        float diff = (sedAmount - transportCap);
        //float depositedSoil = clamp(K_D*diff, 0.0, wHeight)*timeStep; // ensures positive water height
        float depositedSoil = K_D*diff*timeStep; // ensures positive water height

        tHeight = tHeight + depositedSoil;
        wHeight = wHeight - depositedSoil;
        sedAmount = sedAmount - depositedSoil;
    }

    hardnesCoef = max(K_H_MIN, hardnesCoef - timeStep * K_H * K_S * (sedAmountOld - transportCap));

    // WARN: NOT DONE CHECK WHAT NEEDS TO BE UPDATED
    imageStore(albedoHeightSampler, ivec2(coords), vec4(albedoHeight.rgb, tHeight));
    imageStore(waterTextureSampler, ivec2(coords), vec4(wVel.xy, wHeightOld, wHeight));
    imageStore(sedimentSampler, ivec2(coords), vec4(hardnesCoef,0,0,sedAmount));
}
