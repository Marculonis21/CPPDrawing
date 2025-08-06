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

float GGX_Distribution(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = M_PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float a)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - a) + a;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float a)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, a);
    float ggx2 = GeometrySchlickGGX(NdotL, a);
	
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosT, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosT, 5);
}

uint pcg_hash(uint state)
{
    state = state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

void main(){

    vec3 pos = POS/8;
    vec3 albedo = COLOR;
    float metallic = 0.0;
    float roughness = 0.98;
    float ao = 1.0;

    vec3 N = normalize(NORMAL);
    vec3 V = normalize(cameraPos - pos);

    vec3 L = normalize(sunDirection);
    vec3 H = normalize(V+L);

    vec3 lightColor = vec3(1, 0.949, 0.906);

    float NDF = GGX_Distribution(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);

    const vec3 IOR_SAND = vec3(1.5);
    vec3 F0 = abs((1.0 - IOR_SAND)/(1.0 + IOR_SAND));
    F0 = vec3(F0*F0);
    F0 = mix(F0, albedo, metallic);

    vec3 F = FresnelSchlick(dot(H, V), F0);
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    vec3 numerator = NDF*G*F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 diffuse = kD * albedo / M_PI;

    vec3 lighting = (diffuse + specular) * max(dot(N, L), 0.0) * lightColor;
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 _color = ambient + lighting;
    _color = _color / (_color + vec3(1.0));
    _color = pow(_color, vec3(1.0/2.2));
    color = vec4(_color, 1);
}
