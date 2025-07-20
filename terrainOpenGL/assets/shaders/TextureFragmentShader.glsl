#version 450 core

in vec2 UV;
in float HEIGHT;
in vec3 NORMAL;
in vec3 POS;

// Ouput data
in vec3 COLOR;
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D albedoHeightSampler;

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

void main(){

    vec3 _color = COLOR;
    color = vec4(_color, 1);

    vec3 pos = vec3(POS.x, POS.y, POS.z);
    vec3 _sun = vec3(sunPosition.x, sunPosition.y, sunPosition.z);

    color *= max(dot(NORMAL, normalize(_sun - pos)), 0.1);

    color = vec4(color.rgb, 1);
    color = vec4(NORMAL, 1);
}
