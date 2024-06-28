#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D albedoHeightSampler;

uniform float waterLevel;
uniform float sandLevel;
uniform float grassLevel;

uniform vec3 sunPosition;

const vec4 sandColor  = vec4(1.0, 0.98, 0.83, 1);
const vec4 grassColor = vec4(0.44, 0.75, 0.28, 1);
const vec4 rockColor  = vec4(0.33,0.35,0.36, 1);

float get_height(vec2 coords)
{
    return imageLoad(albedoHeightSampler, ivec2(coords)).w;
}

float hash(vec2 uv)
{
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

/* vec4 get_shadows(vec4 color, vec3 startPos, vec3 sunPos) */
/* { */
/*     //what about doing shadows the opposite way?? FROM SUN TO THE TERRAIN - */
/*     //might be better for the occlusion */

/*     startPos.x = startPos.x / tTextureSize; */
/*     startPos.z = startPos.z / tTextureSize; */
    
/*     const float maxSteps = 300; */
/*     const float minStep = 1/512.0; */

/*     startPos.y = startPos.y; */

/*     vec3 dir = normalize(sunPos - startPos); */
/*     /1* dir.y = dir.y; *1/ */
/*     vec3 pos = startPos; */
/*     float height = pos.y; */

/*     for (int i = 1; i < maxSteps; i++) */
/*     { */
/*         pos += dir*max((pos.y-height)*0.01, minStep); */

/*         if(pos.y > 1 || pos.x > 1 || pos.z > 1 || pos.x < 0 || pos.z < 0) { */
/*             break; */
/*         } */

/*         height = get_height(pos.xz*tTextureSize); */

/*         if(height >= pos.y) { */
/*             color.rgb = color.rgb * (1.0-((maxSteps-i)/(maxSteps*1.5))); */
/*             color.a = color.a     / (1.0-((maxSteps-i)/(maxSteps*1.3))); */
/*             break; */
/*         } */
/*     } */
/*     return color; */
/* } */

vec4 get_color(float height, vec2 uv, vec3 sunPos)
{
    vec4 color = vec4(0);

    if(height <= waterLevel-0.005) {
        color = vec4(mix(vec4(0,0,0.4,1), sandColor, exp(-1.2*waterLevel/height)));
    }
    else {

      float _height = height + (hash(uv)*2 - 1)*0.01;

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

    /* color = get_shadows(color, vec3(uv.x, height, uv.y), sunPos); */
    return color;
}

/* vec3 get_normal(vec2 uv) */
/* { */
/*     // amount of quad * tess factor */
/*     const float step = 1; */
/*     vec3 vertex = vec3(uv.x, 0, uv.y); */
/*     vec3 UP    = vec3(uv.x, 0, uv.y+step); */
/*     vec3 RIGHT = vec3(uv.x+step, 0, uv.y); */

/*     vertex.y = tTextureSize*get_height(vertex.xz); */
/*     UP.y =     tTextureSize*get_height(UP.xz); */
/*     RIGHT.y =  tTextureSize*get_height(RIGHT.xz); */

/*     return -normalize(cross(RIGHT-vertex,UP-vertex)); */
/* } */

void main()
{
    vec2 coords = gl_GlobalInvocationID.xy;
    vec3 sunPos = vec3(sunPosition.x, sunPosition.y, sunPosition.z);

    float height = get_height(coords);
    vec4 albedoHeight = vec4(get_color(height,coords, sunPos).rgb, height);

    imageStore(albedoHeightSampler, ivec2(coords), albedoHeight);
}
