#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 vertexPos;
in vec3 normal;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D albedoHeightSampler;
uniform sampler2D normalSampler;
uniform vec3 sunPosition;
uniform float waterLevel;
uniform float sandLevel;
uniform float grassLevel;
uniform vec3 cameraPos;

const float HEIGHTMULT = 5.0f;

float get_height(vec2 uv)
{
    if (uv.x > 1 || uv.x < 0 || uv.y > 1 || uv.y < 0)
        return -10;

    return HEIGHTMULT*texture(albedoHeightSampler, uv).w;
}

vec4 get_shadows(vec4 color, vec3 startPos, vec3 sunPos)
{
    const float maxSteps = 200;
    const float minStep = 0.0025;

    startPos.y *= HEIGHTMULT;

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
            color.rgb = color.rgb * (1.0-((maxSteps-i)/(maxSteps*1.5)));
            color.a = color.a     / (1.0-((maxSteps-i)/(maxSteps*1.3)));
            break;
        }
    }

    return color;
}

vec3 get_terrain_color(vec2 uv)
{
    return texture(albedoHeightSampler, uv).rgb;
}


vec4 get_reflection(vec3 startPos, vec3 dir)
{
    const float maxSteps = 150;
    const float step = 0.01;

    vec3 pos = startPos + dir*0.001;

    for (int i = 0; i < maxSteps; i++)
    {
        pos += dir*step;

        if(pos.y > 1*HEIGHTMULT) {
            break;
        }

        float height = get_height(pos.xz);

        if(height > pos.y)
        {
            /* return vec4(1,0,0,1); */
            return vec4(get_terrain_color(pos.xz),1);
        }
    }

    return vec4(0.2,0.2,0.5,1);
}

void main(){

    vec3 POS = vec3(UV.x, vertexPos.y, UV.y);
    vec3 _sun = vec3(sunPosition.x, sunPosition.y*HEIGHTMULT, sunPosition.z);

    vec3 camDir = normalize(vertexPos-cameraPos);
    vec3 camReflect = reflect(camDir,normal);

    camReflect.y *= 5;
    camReflect = normalize(camReflect);

    color = get_reflection(POS, camReflect);
    color.a = clamp(1-dot(-camDir,normal),0,1);
    color = get_shadows(color, POS, _sun);
}
