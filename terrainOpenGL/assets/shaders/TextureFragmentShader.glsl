#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
flat in vec3 COLOR;
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform sampler2D albedoSampler;
uniform vec3 sunPosition;

float heightScale = 1;

float get_height(vec2 uv)
{
    if (uv.x > 1 || uv.x < 0)
        return 0;
    if (uv.y > 1 || uv.y < 0)
        return 0;

    return heightScale*texture(heightMapSampler, uv).r;
}

vec3 get_normal(vec2 uv)
{
    const float step = 1.0f/1.0f;
    vec3 vertex = vec3(uv.x, 0, uv.y);
    vec3 UP    = vec3(uv.x, 0, uv.y+step);
    vec3 RIGHT = vec3(uv.x+step, 0, uv.y);

    vertex.y = get_height(vertex.xz);
    UP.y = get_height(UP.xz);
    RIGHT.y = get_height(RIGHT.xz);

    return normalize(cross(RIGHT-vertex,UP-vertex));
}

vec4 get_shadows(vec4 color, vec3 startPos)
{
    const float maxSteps = 200;
    const float step = 0.005;

    vec3 dir = normalize(sunPosition - startPos);

    for (int i = 1; i < maxSteps; i++)
    {
        vec3 pos = startPos + (dir*i*step);

        if(get_height(pos.xz) > pos.y)
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

    float waterLevel = 0.4;
    float height = get_height(UV);
    if(height <= waterLevel) {
        color = vec4(mix(vec3(0,0,0.5), _color, exp(-1.2*waterLevel/height)),1);
    }
    else {
        vec3 POS = vec3(UV.x, height, UV.y);
        color = get_shadows(vec4(_color,1), POS);
    }

    color = vec4(color.rgb,1);
}
