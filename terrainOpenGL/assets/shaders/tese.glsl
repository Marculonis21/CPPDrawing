#version 450 core

layout (quads, equal_spacing, ccw) in;

in vec2 uv[];
out vec2 UV;
out vec3 POS;
flat out vec3 COLOR;

uniform mat4 MVP;

uniform sampler2D albedoSampler;
uniform sampler2D heightMapSampler;

float get_height(vec2 uv)
{
    return texture(heightMapSampler, uv).r;
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 uv0 = uv[0];
    vec2 uv1 = uv[1];
    vec2 uv2 = uv[2];
    vec2 uv3 = uv[3];

    vec2 leftUV = uv0 + v*(uv3 - uv0);
    vec2 rightUV = uv1 + v*(uv2 - uv1);
    vec2 texCoord = leftUV + u * (rightUV - leftUV);

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    vec4 leftPos = pos0 + v * (pos3 - pos0);
    vec4 rightPos = pos1 + v * (pos2 - pos1);
    vec4 _pos = leftPos + u * (rightPos - leftPos);

    gl_Position = MVP * vec4(_pos.xyz + vec3(0,2,0)*get_height(texCoord), 1);

    UV = texCoord;
    POS = gl_Position.xyz;
    COLOR = texture(albedoSampler, UV).rgb;
}
