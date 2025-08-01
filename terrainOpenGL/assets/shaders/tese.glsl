#version 450 core

layout (quads, equal_spacing, ccw) in;

in vec2 uv[];
out vec2 UV;
out vec3 POS;
out float HEIGHT;
out vec3 NORMAL;
out vec3 COLOR;

uniform mat4 MVP;

layout(binding = 0) uniform sampler2D albedoHeightSampler;
layout(binding = 1) uniform sampler2D normalSampler;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 uv0 = uv[0];
    vec2 uv1 = uv[1];
    vec2 uv2 = uv[2];
    vec2 uv3 = uv[3];

    vec2 leftUV = mix(uv0, uv3, v);
    vec2 rightUV = mix(uv1, uv2, v);
    vec2 texCoord = mix(leftUV, rightUV, u);

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    vec4 leftPos = mix(pos0, pos3, v);
    vec4 rightPos = mix(pos1, pos2, v);
    vec4 _pos = mix(leftPos, rightPos, u);

    UV = texCoord;
    vec4 albedoHeight = texture(albedoHeightSampler, UV);
    vec4 normal = texture(normalSampler, UV);

    NORMAL = normal.rgb;

    COLOR = albedoHeight.rgb;
    HEIGHT = albedoHeight.w;

    POS = _pos.xyz + vec3(0,1,0)*HEIGHT;
    gl_Position = MVP * vec4(POS, 1);
}
