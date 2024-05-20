#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;
out vec3 vertexPos;
out vec3 normal;
out float HEIGHTMULT;

uniform mat4 MVP;
uniform float waterLevel;
uniform float time;

const float heightMult = 5.0f;

float get_offset(vec2 uv)
{
    return 0.002 * (sin(time*2+uv.x*100) + cos(time + uv.y*100));
}

vec3 get_normal(vec2 uv)
{
    const float step = 0.1f;
    vec3 vertex = vec3(uv.x,      0, uv.y);
    vec3 UP     = vec3(uv.x,      0, uv.y+step);
    vec3 RIGHT  = vec3(uv.x+step, 0, uv.y);

    vertex.y = get_offset(vertex.xz);
    UP.y     = get_offset(UP.xz);
    RIGHT.y  = get_offset(RIGHT.xz);

    return normalize(cross(UP-vertex,RIGHT-vertex));
}


void main(){
    float offset = get_offset(vertexUV);

    vertexPos = vertexPosition_modelspace + vec3(0,1,0)*waterLevel*heightMult + vec3(0,1,0)*offset;
	gl_Position = MVP*vec4(vertexPos,1);
    
	UV = vertexUV;
    normal = get_normal(vertexUV);

    HEIGHTMULT = heightMult;
}

