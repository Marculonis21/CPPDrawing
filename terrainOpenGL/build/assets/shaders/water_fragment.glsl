#version 450 core

in vec2 UV;
in vec3 POS;
in vec3 NORMAL;

// Ouput data
/* flat in vec3 COLOR; */
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D albedoHeightSampler;
/* uniform sampler2D normalSampler; */
uniform sampler2D waterTextureSampler;
uniform sampler2D sedimentSampler;
/* uniform sampler2D waterFlowSampler; */

uniform vec3 cameraPos;

const float sizeOfMesh = 10.0;
const float HEIGHTMULT = 5.0;

float get_height(vec2 uv)
{
    return texture(albedoHeightSampler, uv).w;
}

vec3 get_terrain_color(vec2 uv)
{
    return texture(albedoHeightSampler, uv).rgb;
}

/* vec4 get_shadows(vec4 color, vec3 startPos, vec3 sunPos) */
/* { */
/*     //what about doing shadows the opposite way?? FROM SUN TO THE TERRAIN - */
/*     //might be better for the occlusion */
    
/*     const float maxSteps = 200; */
/*     const float minStep = 0.0025; */

/*     vec3 dir = normalize(sunPos - startPos); */
/*     vec3 pos = startPos + dir*0.001; */
/*     float height = pos.y; */

/*     for (int i = 1; i < maxSteps; i++) */
/*     { */
/*         /1* pos += dir*max((pos.y-height)*0.05, minStep); *1/ */
/*         pos += dir*minStep; */

/*         height = HEIGHTMULT*get_height(pos.xz); */

/*         if(height >= pos.y) */
/*         { */
/*             /1* color.rgb = color.rgb * (1.0-((maxSteps-i)/(maxSteps*1.5))); *1/ */
/*             color.rgb = color.rgb * 0.5; */
/*             color.a = color.a     / (1.0-((maxSteps-i)/(maxSteps*1.3))); */
/*             break; */
/*         } */
/*     } */

/*     return color; */
/* } */

float get_depth(vec3 startPos, vec3 dir)
{
    vec3 pos = startPos;
    float height = 0;
    const float step = 0.01;
    const int maxSteps = 50;

    for (int i = 1; i < maxSteps; i++) {
        pos += dir * step;

        height = HEIGHTMULT*get_height(pos.xz/sizeOfMesh);

        if(height >= pos.y){
            return step*i;
        }
    }
    return maxSteps*step;
}

vec4 get_reflection(vec3 startPos, vec3 dir)
{
    const float maxSteps = 200;
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
    /* vec3 pos = vec3(POS.x/sizeOfMesh, POS.y, POS.z/sizeOfMesh); */

    /* vec3 camDir = normalize(POS-cameraPos); */
    /* vec3 camReflect = reflect(camDir,NORMAL); */

    /* camReflect.y *= 5; */
    /* camReflect = normalize(camReflect); */

    /* color = get_reflection(POS, camReflect); */

    float depth = get_depth(POS, normalize(POS-cameraPos));
    depth = exp(depth*2);
    color = vec4(0.2,0.2,0.5,depth/3);

    float sed = texture(sedimentSampler, UV).w*10;
    /* color = sed > 0.01 ? vec4(1,0,0, 1) : vec4(0,1,0,1); */
    color = vec4(sed,sed,sed,1);
    /* color = vec4(NORMAL,1); */

    /* color.a = clamp(1-dot(-camDir,NORMAL),0,1); */
    /* color = get_shadows(color, POS, _sun); */
}
