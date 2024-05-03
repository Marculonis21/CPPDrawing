#version 450 core

layout (quads, equal_spacing, ccw) in;

in vec2 uv[];
out vec2 UV;
out vec3 POS;
out float HEIGHT;
/* out float HEIGHTMULT; */
out vec3 NORMAL;
flat out vec3 COLOR;

uniform mat4 MVP;

uniform sampler2D albedoHeightSampler;
uniform sampler2D normalSampler;

const float heightMult = 4.0;

/* float get_height(vec2 uv) */
/* { */
/*     return texture(heightMapSampler, uv).r; */
/* } */

/* vec3 get_color(float height, vec2 uv, vec3 normal) */
/* { */
/*     /1* height -= waterLevel; *1/ */
/*     float steepness = pow(dot(vec3(0, 1, 0), normal),1); */

/*     /1* steepness += (hash(uv)*2 - 1)*0.1; *1/ */
/*     /1* if(steepness > 0.8) *1/ */
/*     /1*     return vec3(1,0,0); *1/ */
/*     /1* if(steepness < 0.1) *1/ */
/*     /1*     return vec3(0,1,0); *1/ */
/*     /1* return vec3(steepness); *1/ */

/*     height += (hash(uv)*2 - 1)*0.01; */

/*     /1* if (steepness < 0.2) { *1/ */
/*     /1*     return vec3(0.33,0.35,0.36); *1/ */
/*     /1* } *1/ */

/*     if (height < sandLevel) { */
/*         return vec3(1.0, 0.98, 0.83); */
/*     } */
/*     else if (height < grassLevel) { */
/*         /1* if(steepness > 0.8) *1/ */
/*         /1* return vec3(82/255.0, 152/255.0, 47/255.0); *1/ */
/*         return vec3(0.44, 0.75, 0.28); */
/*     } */
/*     return vec3(0.33,0.35,0.36); */
/* } */

/* vec3 get_normal(vec2 uv) */
/* { */
/*     // amount of quad * tess factor */
/*     const float step = 1.0f/640.0f; */
/*     vec3 vertex = vec3(uv.x, 0, uv.y); */
/*     vec3 UP    = vec3(uv.x, 0, uv.y+step); */
/*     vec3 RIGHT = vec3(uv.x+step, 0, uv.y); */

/*     vertex.y = heightMult*get_height(vertex.xz); */
/*     UP.y =     heightMult*get_height(UP.xz); */
/*     RIGHT.y =  heightMult*get_height(RIGHT.xz); */

/*     return normalize(cross(RIGHT-vertex,UP-vertex)); */
/* } */

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 uv0 = uv[0];
    vec2 uv1 = uv[1];
    vec2 uv2 = uv[2];
    vec2 uv3 = uv[3];

    /* vec2 leftUV = uv0 + v*(uv3 - uv0); */
    /* vec2 rightUV = uv1 + v*(uv2 - uv1); */
    vec2 leftUV = mix(uv0, uv3, v);
    vec2 rightUV = mix(uv1, uv2, v);
    /* vec2 texCoord = leftUV + u * (rightUV - leftUV); */
    vec2 texCoord = mix(leftUV, rightUV, u);

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    /* vec4 leftPos = pos0 + v * (pos3 - pos0); */
    /* vec4 rightPos = pos1 + v * (pos2 - pos1); */
    /* vec4 _pos = leftPos + u * (rightPos - leftPos); */
    vec4 leftPos = mix(pos0, pos3, v);
    vec4 rightPos = mix(pos1, pos2, v);
    vec4 _pos = mix(leftPos, rightPos, u);

    /* float height = get_height(texCoord); */
    UV = texCoord;
    vec4 albedoHeight = texture(albedoHeightSampler, UV);
    vec4 normal = texture(normalSampler, UV);
    NORMAL = normal.rgb;
    HEIGHT = albedoHeight.w;
    COLOR = albedoHeight.rgb;

    POS = _pos.xyz + vec3(0,1,0)*HEIGHT*heightMult;
    gl_Position = MVP * vec4(POS, 1);
    /* HEIGHTMULT = heightMult; */
}
