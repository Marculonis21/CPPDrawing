#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;
in float HEIGHT;
in vec3 NORMAL;
in vec3 POS;

// Ouput data
flat in vec3 COLOR;
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform vec3 sunPosition;
uniform float waterLevel;

float get_height(vec2 uv)
{
    if (uv.x > 1 || uv.x < 0 || uv.y > 1 || uv.y < 0)
        return 0;

    return texture(heightMapSampler, uv).r;
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

    const float sizeOfMesh = 10.0;

    vec3 _color = COLOR;
    color = vec4(_color, 1);

    vec3 pos = vec3(POS.x/sizeOfMesh, POS.y, POS.z/sizeOfMesh);

    if(HEIGHT <= waterLevel-0.005) {
        color = vec4(mix(vec3(0,0,0.5), _color, exp(-1.2*waterLevel/HEIGHT)),1);
    }
    else {
        color = get_shadows(vec4(_color,1), pos);
    }

    color *= max(dot(NORMAL, normalize(sunPosition - pos)), 0.8);
    color = vec4(color.rgb, 1);
    /* color = vec4(NORMAL, 1); */
}
