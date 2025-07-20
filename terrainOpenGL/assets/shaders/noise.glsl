#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D albedoHeightSampler;
layout(rgba32f, binding = 1) uniform image2D normalSampler;

uniform int tTextureSize;
uniform int octaves;
uniform float sFreq;

uniform float waterLevel;
uniform float sandLevel;
uniform float grassLevel;

uniform vec3 sunPosition;

const vec4 sandColor = vec4(1.0, 0.98, 0.83, 1);
const vec4 grassColor = vec4(0.44, 0.75, 0.28, 1);
const vec4 rockColor = vec4(0.33, 0.35, 0.36, 1);

vec4 mod289(vec4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
    return mod289(((x * 34.0) + 10.0) * x);
}

vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

// Classic Perlin noise
float cnoise(vec2 P)
{
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod289(Pi); // To avoid truncation effects in permutation
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;

    vec4 i = permute(permute(ix) + iy);

    vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0;
    vec4 gy = abs(gx) - 0.5;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;

    vec2 g00 = vec2(gx.x, gy.x);
    vec2 g10 = vec2(gx.y, gy.y);
    vec2 g01 = vec2(gx.z, gy.z);
    vec2 g11 = vec2(gx.w, gy.w);

    vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;

    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));

    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
}

// Classic Perlin noise, periodic variant
float pnoise(vec2 P, vec2 rep)
{
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod(Pi, rep.xyxy); // To create noise with explicit period
    Pi = mod289(Pi); // To avoid truncation effects in permutation
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;

    vec4 i = permute(permute(ix) + iy);

    vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0;
    vec4 gy = abs(gx) - 0.5;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;

    vec2 g00 = vec2(gx.x, gy.x);
    vec2 g10 = vec2(gx.y, gy.y);
    vec2 g01 = vec2(gx.z, gy.z);
    vec2 g11 = vec2(gx.w, gy.w);

    vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;

    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));

    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
}

float get_noise(vec2 coords) {
    float sum = 0;

    float ampSum = 0;
    for (int i = 0; i < octaves; i++)
    {
        float amp = pow(0.5, i);
        ampSum += amp;
        float freq = sFreq / (i + 1);
        float value = amp * ((cnoise(coords / freq) + 1.0) / 2.0);
        sum += value;
    }

    sum /= ampSum;
    sum = pow(sum, 4) * 4;

    return sum;
}

float get_height(vec2 coords)
{
    // random choosen div
    float warp_x = get_noise(vec2(coords.x / 2.0, coords.y/1.5));
    float warp_y = get_noise(coords);

    vec2 warp = vec2(warp_x, warp_y);

    return get_noise(coords + warp*25.0);
}

float hash(vec2 uv)
{
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 get_shadows(vec4 color, vec3 startPos, vec3 sunPos)
{
    //what about doing shadows the opposite way?? FROM SUN TO THE TERRAIN -
    //might be better for the occlusion

    startPos.x = startPos.x / tTextureSize;
    startPos.z = startPos.z / tTextureSize;

    const float maxSteps = 300;
    const float minStep = 1 / 512.0;

    startPos.y = startPos.y;

    vec3 dir = normalize(sunPos - startPos);
    /* dir.y = dir.y; */
    vec3 pos = startPos;
    float height = pos.y;

    for (int i = 1; i < maxSteps; i++)
    {
        pos += dir * max((pos.y - height) * 0.01, minStep);

        if (pos.y > 1 || pos.x > 1 || pos.z > 1 || pos.x < 0 || pos.z < 0) {
            break;
        }

        height = get_height(pos.xz * tTextureSize);

        if (height >= pos.y) {
            color.rgb = color.rgb * (1.0 - ((maxSteps - i) / (maxSteps * 1.5)));
            color.a = color.a / (1.0 - ((maxSteps - i) / (maxSteps * 1.3)));
            break;
        }
    }
    return color;
}

vec4 get_color(float height, vec2 uv, vec3 sunPos)
{
    /* float steepness = pow(dot(vec3(0, 1, 0), normal),1); */

    /* steepness += (hash(uv)*2 - 1)*0.1; */
    /* if(steepness > 0.8) */
    /*     return vec3(1,0,0); */
    /* if(steepness < 0.1) */
    /*     return vec3(0,1,0); */
    /* return vec3(steepness); */

    /* if (steepness < 0.2) { */
    /*     return vec3(0.33,0.35,0.36); */
    /* } */

    vec4 color = vec4(0);

    if (height <= waterLevel - 0.005) {
        color = vec4(mix(vec4(0, 0, 0.4, 1), sandColor, exp(-1.2 * waterLevel / height)));
    }
    else {
        float _height = height + (hash(uv) * 2 - 1) * 0.01;

        if (_height < sandLevel) {
            color = sandColor;
        }
        else if (_height < grassLevel) {
            color = grassColor;
        }
        else {
            color = rockColor;
        }
    }

    color = get_shadows(color, vec3(uv.x, height, uv.y), sunPos);
    return color;
}

vec3 get_normal(vec2 uv, vec4 albedoHeight)
{
    // amount of quad * tess factor
    const float step = 1;
    vec3 vertex = vec3(uv.x, 0, uv.y);
    vec3 UP = vec3(uv.x, 0, uv.y + step);
    vec3 RIGHT = vec3(uv.x + step, 0, uv.y);

    vertex.y = tTextureSize * get_height(vertex.xz);
    UP.y = tTextureSize * get_height(UP.xz);
    RIGHT.y = tTextureSize * get_height(RIGHT.xz);

    return -normalize(cross(RIGHT - vertex, UP - vertex));
}

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;
    vec3 sunPos = vec3(sunPosition.x, sunPosition.y, sunPosition.z);

    float height = get_height(coords);
    vec4 albedoHeight = vec4(get_color(height, coords, sunPos).rgb, height);
    vec3 normal = get_normal(coords, albedoHeight);

    imageStore(albedoHeightSampler, ivec2(coords), albedoHeight);
    imageStore(normalSampler, ivec2(coords), vec4(normal, 1.0));
}
