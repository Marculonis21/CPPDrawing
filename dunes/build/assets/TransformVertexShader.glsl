#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
/* layout(location = 1) in vec2 vertexUV; */

// Output data ; will be interpolated for each fragment.
/* out vec2 UV; */

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

	// Output position of the vertex, in clip space : MVP * position
    vec3 offset = vec3(0,0,1)*sin(vertexPosition_modelspace.x*3);
	gl_Position =  MVP * vec4(vertexPosition_modelspace + offset,1);
	
	// UV of the vertex. No special space for this one.
	/* UV = vertexUV; */
}

