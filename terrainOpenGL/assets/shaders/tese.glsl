#version 450 core

layout (quads, equal_spacing, ccw) in;

in vec2 uv[];
out vec2 UV;
out vec3 POS;
out float HEIGHT;
out vec3 NORMAL;
flat out vec3 COLOR;

uniform mat4 MVP;

uniform sampler2D heightMapSampler;

uniform float waterLevel;
uniform float sandLevel;
uniform float grassLevel;

const float heightMult = 4.0;

float get_height(vec2 uv)
{
    return texture(heightMapSampler, uv).r;
}

float hash(vec2 uv)
{
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 get_color(float height, vec2 uv, vec3 normal)
{
    /* height -= waterLevel; */
    float steepness = pow(dot(vec3(0, 1, 0), normal),1);

    /* steepness += (hash(uv)*2 - 1)*0.1; */
    /* if(steepness > 0.8) */
    /*     return vec3(1,0,0); */
    /* if(steepness < 0.1) */
    /*     return vec3(0,1,0); */
    /* return vec3(steepness); */

    height += (hash(uv)*2 - 1)*0.05;

    /* if (steepness < 0.2) { */
    /*     return vec3(0.33,0.35,0.36); */
    /* } */

    if (height < sandLevel) {
        return vec3(1.0, 0.98, 0.83);
    }
    else if (height < grassLevel) {
        /* if(steepness > 0.8) */
        /* return vec3(82/255.0, 152/255.0, 47/255.0); */
        return vec3(0.44, 0.75, 0.28);
    }
    return vec3(0.33,0.35,0.36);
}

vec3 get_normal(vec2 uv)
{
    // amount of quad * tess factor
    const float step = 1.0f/640.0f;
    vec3 vertex = vec3(uv.x, 0, uv.y);
    vec3 UP    = vec3(uv.x, 0, uv.y+step);
    vec3 RIGHT = vec3(uv.x+step, 0, uv.y);

    vertex.y = heightMult*get_height(vertex.xz);
    UP.y =     heightMult*get_height(UP.xz);
    RIGHT.y =  heightMult*get_height(RIGHT.xz);

    return normalize(cross(RIGHT-vertex,UP-vertex));
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

    float height = get_height(texCoord);
    HEIGHT = height;
    POS = _pos.xyz + vec3(0,1,0)*height*heightMult;
    gl_Position = MVP * vec4(POS, 1);
    UV = texCoord;
    NORMAL = -get_normal(UV);
    COLOR = get_color(height,UV,NORMAL);
    /* COLOR = texture(heightMapSampler, UV).rgb; */
}
