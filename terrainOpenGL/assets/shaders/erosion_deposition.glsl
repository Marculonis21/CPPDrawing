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

uniform float K_C // sediment transport - scaling constant
uniform float ALPHA_MIN // min local tilt
uniform float K_DMAX // min local tilt
uniform float K_S; // sed dissolve const
uniform float K_D; // sed deposit const
uniform float K_H; // sed deposit const
uniform float K_H_MIN; // sed deposit const

// ramp function l_MAX
float l_max(float val) {
    return (val >= K_DMAX) ? 1.0 : (1.0 - (K_DMAX - val)/K_DMAX);
}

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;

    // if(waterTexture.w < 0.001) return;

    float tHeight = imageLoad(albedoHeightSampler, ivec2(coords)).w;
    vec3 tNormal = imageLoad(normalSampler, ivec2(coords)).rgb;

    vec4 waterTexture = imageLoad(waterTextureSampler, ivec2(coords));

    vec2 wVel = waterTexture.xy;
    float wHeightOld = waterTexture.z;
    float wHeight = waterTexture.w;

    // float dotV = dot(tNormal, vec3(0,1,0)); 
    // float tilt = sqrt(1 - dotV*dotV);
    
    float alpha = acos(dot(tNormal, vec3(0,1,0))); // local tilt

    float transportCap = K_C * sin(max(ALPHA_MIN, alpha)) * length(wVel) * l_max(wHeightOld); 

    float sedimentTexture = imageLoad(sedimentSampler, ivec2(coords));

    float sedAmount = sedimentTexture.w;
    float _sedAmount = sedimentTexture.w;
    float hardnesCoef = sedimentTexture.x;

    if(transportCap > sedAmount) {
        float diff = (transportCap - sedAmount);
        float dissolvedSoil = K_S*hardnesCoef*diff;

        tHeight = tHeight - dissolvedSoil;
        wHeight = wHeight + dissolvedSoil;
        sedAmount = sedAmount + dissolvedSoil;
    }
    else {
        float diff = (sedAmount - transportCap);
        float depositedSoil = clamp(K_D*diff, 0.0, wHeight); // ensures positive water height

        tHeight = tHeight + depositedSoil;
        wHeight = wHeight - depositedSoil;
        sedAmount = sedAmount - depositedSoil;
    }

    hardnesCoef = max(K_H_MIN, hardnesCoef - timeStep * K_H * K_S * (_sedAmount - transportCap));

    // WARN: NOT DONE CHECK WHAT NEEDS TO BE UPDATED
    imageStore(albedoHeightSampler, ivec2(coords), vec4(albedoHeight.rgb, tHeight));
    imageStore(sedimentSampler, ivec2(coords), vec4(hardnesCoef,0,0,sedAmount));
}
