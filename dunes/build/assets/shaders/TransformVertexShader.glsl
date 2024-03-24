#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 POS;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform sampler2D albedoSampler;
uniform mat4 MVP;

float get_height(vec2 v_UV)
{
    float height = texture(heightMapSampler, v_UV).r;
    /* height *= 3; */

    return height;
}

void main(){

    vec3 vertex = vec3(0,1,0)*get_height(vertexUV);

	gl_Position =  MVP * vec4(vertexPosition_modelspace + vertex,1);
    
    POS = vertexPosition_modelspace + vertex;

	UV = vertexUV;
}

