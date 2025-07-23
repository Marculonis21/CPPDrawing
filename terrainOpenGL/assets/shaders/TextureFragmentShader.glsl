#version 450 core

in vec2 UV;
in float HEIGHT;
in vec3 NORMAL;
in vec3 POS;

// Ouput data
in vec3 COLOR;
out vec4 color;

// Values that stay constant for the whole mesh.
layout(binding = 0) uniform sampler2D albedoHeightSampler;
layout(binding = 2) uniform sampler2D waterTextureSampler;
layout(binding = 3) uniform sampler2D waterFlowSampler;
layout(binding = 4) uniform sampler2D sedimentSampler;

uniform vec3 sunPosition;
uniform float waterLevel;

float get_height(vec2 uv)
{
    return texture(albedoHeightSampler, uv).w;
}

vec4 get_shadows(vec4 color, vec3 startPos, vec3 sunPos)
{
    //what about doing shadows the opposite way?? FROM SUN TO THE TERRAIN -
    //might be better for the occlusion
    
    const float maxSteps = 200;
    const float minStep = 0.0025;

    vec3 dir = normalize(sunPos - startPos);
    vec3 pos = startPos + dir*0.001;
    float height = pos.y;

    for (int i = 1; i < maxSteps; i++)
    {
        /* pos += dir*max((pos.y-height)*0.05, minStep); */
        pos += dir*minStep;

        height = get_height(pos.xz);

        if(height >= pos.y)
        {
            /* color.rgb = color.rgb * (1.0-((maxSteps-i)/(maxSteps*1.5))); */
            color.rgb = color.rgb * 0.5;
            color.a = color.a     / (1.0-((maxSteps-i)/(maxSteps*1.3)));
            break;
        }
    }

    return color;
}

float l_max(float val) {
    const float K_DMAX = 4.0;
    return (val >= K_DMAX) ? 1.0 : (1.0 - (K_DMAX - val)/K_DMAX);
}

void main(){

    vec3 _color = COLOR;
    color = vec4(_color, 1);

    //const float SIZE_MULTIPLIER = 8.0; // number of mesh quads

    color *= max(dot(NORMAL, normalize(sunPosition - POS)), 0.1);

    color = vec4(color.rgb, 1);
    //color = vec4(NORMAL, 1);

    float alpha = dot(NORMAL, vec3(0,1,0)); // local tilt
    vec4 waterTexture = texture(waterTextureSampler, UV);
    vec2 wVel = waterTexture.xy;
    float wHeightOld = waterTexture.z;
    float transportCap = 1 * alpha * length(wVel) * l_max(wHeightOld); 
    // color = vec4(vec3(transportCap), 1);

    float sedAmount = texture(sedimentSampler, UV).w;

    // if(transportCap > sedAmount) {
    //     color = vec4(0,1,0,1);
    // }
    // else {
    //     color = vec4(1,0,0,1);
    // }

    // color = vec4(0,1-sedAmount, 0, 1);
    float diff = (sedAmount - transportCap);
    float depositedSoil = clamp(0.01*diff, 0.0, waterTexture.w)*0.005; // ensures positive water height
    color = vec4(depositedSoil*10, 0,0, 1);
}
