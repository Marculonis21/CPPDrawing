#version 330 core

in vec4 gl_FragCoord;

out vec4 fragColor;

uniform vec2 resolution;
uniform vec3 cameraOrigin;

const float FOV = 1;
const int MAX_STEPS = 100;
const float MAX_DIST = 50;
const float EPSILON = 0.0001;

vec2 sdf(vec3 testPos)
{
    testPos = mod(testPos, 4) - 2;
    vec3 spherePos = vec3(0,0,2);
    float radius = 1;
    float sphereID = 1;

    float distance = length(spherePos-testPos)-radius;

    return vec2(distance, sphereID);
}

vec3 raymarch(vec3 rayDir)
{
    vec2 hit, object = vec2(0,0);

    int steps;
    for(steps=0; steps < MAX_STEPS; steps++)
    {
        vec3 testPos = cameraOrigin + object.x*rayDir;
        hit = sdf(testPos);
        object.x += hit.x;
        object.y = hit.y;
        
        if(abs(hit.x) < EPSILON || object.x > MAX_DIST)
            break;
    }

    /* return object; */
    return vec3(object, steps);
}

vec4 render(vec2 uv)
{
    vec3 rayDir = normalize(vec3(uv, 1));

    vec3 object = raymarch(rayDir);

    if(object.x < MAX_DIST)
    {
        vec3 color = vec3(3.0/object.x);
        return vec4(color,1.0);
    }
    return vec4(0,0,0,1);
    /* return vec4(object.z/MAX_DIST,0,0,1); */
}

void main()
{
    vec2 uv = (2.0*gl_FragCoord.xy - resolution.xy) / resolution.x;

    fragColor = render(uv);
}
