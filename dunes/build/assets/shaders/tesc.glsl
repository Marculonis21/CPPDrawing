#version 450 core

layout (vertices = 4) out;

in vec2 UV[];
/* in vec3 POS[]; */
out vec2 uv[];
/* out vec3 pos[]; */

uniform vec3 cameraPos;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    uv[gl_InvocationID] = UV[gl_InvocationID];
    /* pos[gl_InvocationID] = POS[gl_InvocationID]; */

    if(gl_InvocationID != 0) return;

    vec4 c1 = gl_in[0].gl_Position + (gl_in[3].gl_Position - gl_in[0].gl_Position)/2.0;
    vec4 c2 = gl_in[1].gl_Position + (gl_in[0].gl_Position - gl_in[1].gl_Position)/2.0;
    vec4 c3 = gl_in[2].gl_Position + (gl_in[1].gl_Position - gl_in[2].gl_Position)/2.0;
    vec4 c4 = gl_in[3].gl_Position + (gl_in[2].gl_Position - gl_in[3].gl_Position)/2.0;

    float dist0 = length(vec4(cameraPos,1) - c1);
    float dist1 = length(vec4(cameraPos,1) - c2);
    float dist2 = length(vec4(cameraPos,1) - c3);
    float dist3 = length(vec4(cameraPos,1) - c4);

    int tes0 = int(mix(1024.0, 2.0, clamp(dist0/9.0, 0.0, 1.0)));
    int tes1 = int(mix(1024.0, 2.0, clamp(dist1/9.0, 0.0, 1.0)));
    int tes2 = int(mix(1024.0, 2.0, clamp(dist2/9.0, 0.0, 1.0)));
    int tes3 = int(mix(1024.0, 2.0, clamp(dist3/9.0, 0.0, 1.0)));

    gl_TessLevelOuter[0] = tes0;
    gl_TessLevelOuter[1] = tes1;
    gl_TessLevelOuter[2] = tes2;
    gl_TessLevelOuter[3] = tes3;

    gl_TessLevelInner[0] = max(tes1, tes3);
    gl_TessLevelInner[1] = max(tes0, tes2);
}
