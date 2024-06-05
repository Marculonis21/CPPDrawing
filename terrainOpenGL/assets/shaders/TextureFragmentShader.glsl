#version 450 core

in vec2 UV;
in float HEIGHT;
in float HEIGHTMULT;
in vec3 NORMAL;
in vec3 POS;

// Ouput data
flat in vec3 COLOR;
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D albedoHeightSampler;
uniform sampler2D normalSampler;

uniform vec3 sunPosition;
uniform float waterLevel;

float get_height(vec2 uv)
{
    if (uv.x > 1 || uv.x < 0 || uv.y > 1 || uv.y < 0)
        return -10;

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

        height = HEIGHTMULT*get_height(pos.xz);

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

    const float sizeOfMesh = 10.0;

    vec3 _color = COLOR;
    color = vec4(_color, 1);

    vec3 pos = vec3(POS.x/sizeOfMesh, POS.y, POS.z/sizeOfMesh);
    vec3 _sun = vec3(sunPosition.x, sunPosition.y*HEIGHTMULT, sunPosition.z);

    if(HEIGHT <= waterLevel-0.005) {
        color = vec4(mix(vec3(0,0,0.5), _color, exp(-1.2*waterLevel/HEIGHT)),1);
    }
    else {
        /* color = get_shadows(vec4(_color,1), pos, _sun); */
        color *= max(dot(NORMAL, normalize(_sun - pos)), 0.1);
    }

    color = vec4(color.rgb, 1);
}
