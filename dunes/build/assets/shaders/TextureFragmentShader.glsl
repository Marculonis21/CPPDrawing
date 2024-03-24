#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 POS;

// Ouput data
flat in vec3 COLOR;
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform sampler2D albedoSampler;

float get_height(vec2 uv)
{
    if (uv.x > 1 || uv.x < 0)
        return 0;
    if (uv.y > 1 || uv.y < 0)
        return 0;

    return texture(heightMapSampler, uv).r;
}

vec3 get_normal(vec2 uv)
{
    vec3 vertex = vec3(uv.x, 0, uv.y);
    vec3 UP    = vec3(uv.x, 0, uv.y+0.01);
    vec3 RIGHT = vec3(uv.x+0.01, 0, uv.y);

    vertex.y = get_height(vertex.xz);
    UP.y = get_height(UP.xz);
    RIGHT.y = get_height(RIGHT.xz);

    return normalize(cross(RIGHT-vertex,UP-vertex));
}

const vec3 sunPosition = vec3(0,1,0);
void main(){
	// Output color = color of the texture at the specified UV
    /* color = texture(albedoSampler, UV).rgb; */
    color = COLOR;

    /* vec3 dir = normalize(sunPosition - POS); */
    /* float step = 0.05; */
    /* for (int i = 5; i < 100; i++) */
    /* { */
    /*     vec3 pos = POS + (dir*i*step); */

    /*     if(get_height(pos.xz/10.0) > pos.y) */
    /*     { */
    /*         color = color * (1.0-((100-i)/150.0)); */
    /*         break; */
    /*     } */
    /* } */

    /* color *= max(dot(dir, -get_normal(POS.xz/10.0)), 0.9); */
}
