#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 vertexPos;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform sampler2D albedoSampler;
uniform vec3 sunPosition;
uniform float seaLevel;
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
            return texture(albedoSampler, pos.xz);
        }
    }

    return vec4(0.2,0.2,1,1);
}

void main(){

    vec3 POS = vec3(UV.x, 0.4, UV.y);

    /* vec3 cam = vec3(cameraPos.x/10, cameraPos.y, cameraPos.z/10); */
    vec3 camReflect = reflect(vertexPos-cameraPos,vec3(0,1,0));

    camReflect.y *= 4;
    camReflect = normalize(camReflect);
    color = get_reflection(POS, camReflect);
    color.a = 0.25;
    color = get_shadows(color, POS);
    /* color = vec4(camReflect,1); */
}
