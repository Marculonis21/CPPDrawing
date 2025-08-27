#version 450 core

layout (quads, equal_spacing, ccw) in;

in vec2 uv[];
out vec2 UV;
out vec3 POS;
out float HEIGHT;
out vec3 NORMAL;
out vec3 COLOR;

out vec3 TANGENT;
out vec3 BITANGENT;

uniform mat4 MVP;

layout(binding = 0) uniform sampler2D albedoHeightSampler;
layout(binding = 1) uniform sampler2D normalSampler;

vec3 computeTangent(vec3 p0, vec3 p1, vec3 p2, vec2 uv0, vec2 uv1, vec2 uv2) {
    vec3 edge1 = p1 - p0;
    vec3 edge2 = p2 - p0;

    vec2 deltaUV1 = uv1 - uv0;
    vec2 deltaUV2 = uv2 - uv0;

    float f = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    return normalize(tangent);
}

void main() { 
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

    NORMAL = normalize(normal.rgb);

    COLOR = albedoHeight.rgb;
    HEIGHT = albedoHeight.w;

    POS = _pos.xyz + vec3(0,1,0)*HEIGHT;


    // Compute tangent and bitangent using triangle v0-v1-v2
    vec3 p0 = pos0.xyz;
    vec3 p1 = pos1.xyz;
    vec3 p2 = pos2.xyz;

    vec2 t0 = uv0;
    vec2 t1 = uv1;
    vec2 t2 = uv2;

    vec3 T = computeTangent(p0, p1, p2, t0, t1, t2);

    BITANGENT = normalize(cross(NORMAL, T)); // Construct bitangent
    TANGENT = normalize(cross(BITANGENT, NORMAL));      // Orthonormalize tangent if needed
                                          
    gl_Position = MVP * vec4(POS, 1);
}
