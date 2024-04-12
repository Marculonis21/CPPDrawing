#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;
in float HEIGHT;
in vec3 NORMAL;

// Ouput data
flat in vec3 COLOR;
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform sampler2D sampler;
uniform vec3 sunPosition;
uniform float waterLevel;

float heightScale = 1;


float get_height(vec2 uv)
{
    if (uv.x > 1 || uv.x < 0)
        return 0;
    if (uv.y > 1 || uv.y < 0)
        return 0;

    return heightScale*texture(heightMapSampler, uv).r;
}

vec4 get_shadows(vec4 color, vec3 startPos)
{
    const float maxSteps = 200;
    const float minStep = 0.001;

    vec3 dir = normalize(sunPosition - startPos);
    vec3 pos = startPos + dir*0.001;
    float height = pos.y;

    for (int i = 1; i < maxSteps; i++)
    {
        pos += dir*max((pos.y-height)*0.05, minStep);

        height = get_height(pos.xz);

        if(height >= pos.y)
        {
            color.rgb = color.rgb * (1.0-((maxSteps-i)/(maxSteps*1.5)));
            color.a = color.a     / (1.0-((maxSteps-i)/(maxSteps*1.3)));
            break;
        }
    }

    return color;
}

void main(){

    vec3 _color = COLOR;
    color = vec4(HEIGHT,HEIGHT,HEIGHT,1);
    return;

    /* return; */
    vec3 POS = vec3(UV.x, HEIGHT, UV.y);

    /* if(HEIGHT <= waterLevel-0.005) { */
    /*     color = vec4(mix(vec3(0,0,0.5), _color, exp(-1.2*waterLevel/HEIGHT)),1); */
    /* } */
    /* else { */
    /*     color = get_shadows(vec4(_color,1), POS); */
    /* } */

    /* color *= max(dot(-get_normal(UV), normalize(sunPosition - POS)), 0.75); */
}
