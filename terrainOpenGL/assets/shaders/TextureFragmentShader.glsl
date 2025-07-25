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
layout(binding = 4) uniform sampler2D sedimentSampler;

uniform vec3 cameraPos;
uniform vec3 sunDirection;
uniform float waterLevel;

float get_height(vec2 uv)
{
    return texture(albedoHeightSampler, uv).w;
}

//vec4 get_shadows(vec4 color, vec3 startPos, vec3 sunPos)
//{
//    //what about doing shadows the opposite way?? FROM SUN TO THE TERRAIN -
//    //might be better for the occlusion
//    
//    const float maxSteps = 200;
//    const float minStep = 0.0025;
//
//    vec3 dir = normalize(sunPos - startPos);
//    vec3 pos = startPos + dir*0.001;
//    float height = pos.y;
//
//    for (int i = 1; i < maxSteps; i++)
//    {
//        /* pos += dir*max((pos.y-height)*0.05, minStep); */
//        pos += dir*minStep;
//
//        height = get_height(pos.xz);
//
//        if(height >= pos.y)
//        {
//            /* color.rgb = color.rgb * (1.0-((maxSteps-i)/(maxSteps*1.5))); */
//            color.rgb = color.rgb * 0.5;
//            color.a = color.a     / (1.0-((maxSteps-i)/(maxSteps*1.3)));
//            break;
//        }
//    }
//
//    return color;
//}
//

#define M_PI 3.1415926535897932384626433832795

float NDF_GGX(vec3 N, vec3 H, float roughness) {
    float NH2 = dot(N,H)*dot(N,H);
    float rougness2 = roughness*roughness;
    float bracket = NH2 * (rougness2 - 1) + 1;

    return rougness2/(M_PI * bracket * bracket);
}

void main(){

    vec3 _pos = POS/8;

    vec3 albedo = COLOR;

    //albedo *= max(dot(NORMAL, normalize(sunDirection - POS)), 0.1);

    float metallic = 0.0;
    float rougness = 0.8;

    vec3 N = NORMAL;
    vec3 V = normalize(cameraPos - _pos);
    vec3 L = normalize(sunDirection);
    vec3 H = normalize(V+L);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    float NDF = NDF_GGX(N, H, rougness);

    color = vec4(albedo, 1);
    color = vec4(vec3(NDF), 1);
    
    //float alpha =  dot(NORMAL, vec3(0,1,0)); 
    //if (alpha > 0.4) {
    //    color = vec4(0,1,0,1);
    //}
    //else {
    //    color = vec4(1,0,0,1);
    //}

    //vec4 sedimentTexture = texture(sedimentSampler, UV);
    //float hardness = sedimentTexture.x;
    //color = vec4(hardness, 0,0,1);
}
