#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform vec3 sunPosition;
uniform float seaLevel;

float get_height(vec2 uv)
{
    if (uv.x > 1 || uv.x < 0)
        return 0;
    if (uv.y > 1 || uv.y < 0)
        return 0;

    return texture(heightMapSampler, uv).r;
}

void main(){
    vec3 _color = vec3(0,0,1);

    /* vec3 POS = vec3(UV.x, seaLevel, UV.y); */
    /* vec3 dir = normalize(sunPosition - POS); */

    /* float step = 0.001; */
    /* for (int i = 5; i < 100; i++) */
    /* { */
    /*     vec3 pos = POS + (dir*i*step); */

    /*     if(get_height(pos.xz) > pos.y) */
    /*     { */
    /*         _color = _color * (1.0-((100-i)/150.0)); */
    /*         break; */
    /*     } */
    /* } */

    color = vec4(_color,0.1);
}
