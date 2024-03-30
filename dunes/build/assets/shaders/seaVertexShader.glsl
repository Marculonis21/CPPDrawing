#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;
out vec3 vertexPos;

uniform mat4 MVP;
uniform float seaLevel;

void main(){

	gl_Position = MVP*vec4(vertexPosition_modelspace + vec3(0,2,0)*seaLevel,1);
	/* gl_Position =  MVP*vec4(vertexPosition_modelspace,1); */
    
	UV = vertexUV;
    vertexPos = vertexPosition_modelspace + vec3(0,2,0)*seaLevel;
}

