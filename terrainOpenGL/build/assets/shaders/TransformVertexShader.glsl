#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

uniform vec3 origin;

void main(){

	gl_Position =  vec4(origin + vertexPosition_modelspace,1);
    
	UV = vertexUV;
}

