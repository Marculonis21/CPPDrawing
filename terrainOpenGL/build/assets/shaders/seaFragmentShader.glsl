#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 vertexPos;
in vec3 normal;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform vec3 sunPosition;
uniform float waterLevel;
uniform float sandLevel;
uniform float grassLevel;
uniform vec3 cameraPos;

float get_height(vec2 uv)
{
    return texture(heightMapSampler, uv).r;
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

vec3 get_terrain_color(float height)
{
    height -= waterLevel;

    if (height < sandLevel) {
        return vec3(1.0, 0.98, 0.83);
    }
    else if (height < grassLevel-waterLevel) {
        return vec3(0.44, 0.75, 0.28);
    }
    return vec3(0.33,0.35,0.36);
}


vec4 get_reflection(vec3 startPos, vec3 dir)
{
    const float maxSteps = 150;
    const float step = 0.01;

    for (int i = 0; i < maxSteps; i++)
    {
        vec3 pos = startPos + dir*0.0001 + (dir*i*step);

        if(pos.y > 1) {
            break;
        }

        float height = get_height(pos.xz);

        if(height > pos.y)
        {
            return vec4(get_terrain_color(height),1);
        }
    }

    return vec4(0.2,0.2,0.5,1);
}
void main(){

    vec3 POS = vec3(UV.x, waterLevel, UV.y);

    vec3 camDir = normalize(vertexPos-cameraPos);
    vec3 camReflect = reflect(camDir,normal);

    camReflect.y *= 4;
    camReflect = normalize(camReflect);

    color = get_reflection(POS, camReflect);
    color.a = clamp(1-dot(-camDir,normal),0,1);
    color = get_shadows(color, POS);
}
