#version 330 core

in vec4 gl_FragCoord;

out vec4 fragColor;

uniform vec2 resolution;

uniform vec3 cameraOrigin;
uniform vec3 cameraLookAt;

const float FOV = 1;
const int MAX_STEPS = 500;
const float MAX_DIST = 50;
const float EPSILON = 0.001;

struct Object
{
    float distance;
    float materialID;
};

Object sdfMin(Object obj1, Object obj2)
{
    if(obj1.distance < obj2.distance)
        return obj1;
    else
        return obj2;
}

Object sdf(vec3 testPos)
{
    vec3 sphere1p = vec3(0,5,2);
    float sphere1r = 1;
    Object sphere1 = Object(length(sphere1p - testPos)-sphere1r, 1);

    vec3 planePos = vec3(0);
    planePos.y = sin(testPos.x) + 5*sin(0.3 * testPos.x);
    Object plane = Object(testPos.y - planePos.y, 2.0);

    Object result = sdfMin(sphere1, plane);
    return result;
}

vec3 calcNormal(vec3 p)
{
    const float eps = 0.0001; 
    const vec2 h = vec2(eps,0);
    return normalize( vec3(sdf(p+h.xyy).distance - sdf(p-h.xyy).distance,
                           sdf(p+h.yxy).distance - sdf(p-h.yxy).distance,
                           sdf(p+h.yyx).distance - sdf(p-h.yyx).distance ) );
}

vec4 raymarch(vec3 rayStart, vec3 rayDir)
{
    Object hit, object = Object(0,0);
    float closestDist = 99999.0;

    int steps;
    for(steps=0; steps < MAX_STEPS; steps++)
    {
        vec3 testPos = rayStart + object.distance*rayDir;
        hit = sdf(testPos);

        if(hit.distance < closestDist) closestDist = hit.distance;

        object.distance += hit.distance;
        object.materialID = hit.materialID;
        
        if(abs(hit.distance) < EPSILON || object.distance > MAX_DIST)
            break;
    }

    return vec4(object.distance, object.materialID, steps, closestDist);
}

float getLight(vec3 hitPos)
{
    vec3 sunDir = normalize(vec3(10,-10,10));
    float sunIntensity = 1;

    /* // offset by normal */
    hitPos += calcNormal(hitPos)*0.1;

    vec4 result = raymarch(hitPos, -sunDir);
    if(result.x > MAX_DIST || result.z >= MAX_STEPS)
    {
        return min(sunIntensity * result.w/0.1, sunIntensity);
    }

    return 0.0;
}

mat3 getCam()
{
    vec3 camForward = normalize(cameraLookAt);
    vec3 camRight = normalize(cross(camForward, vec3(0,1,0)));
    vec3 camUp = normalize(cross(camForward, -camRight));
    return mat3(camRight, camUp, camForward);
}

vec4 render(vec2 uv)
{
    vec3 rayDir = getCam() * normalize(vec3(uv, FOV));

    vec3 object = raymarch(cameraOrigin, rayDir).xyz;

    vec3 background = vec3(0.1,0.1,0.1);
    vec3 color = vec3(0,0,0);

    if(object.x < MAX_DIST)
    {
        if(object.y == 1.0)
            color = vec3(1,0,0);
        else if(object.y == 2.0)
            color = vec3(0,1,0);

        color = mix(color, background, 1-exp(-0.001 * object.x*object.x));
        vec3 hitPos = cameraOrigin + object.x*rayDir;
        color *= getLight(hitPos);;
        color = pow(color, vec3(0.4545));
        return vec4(color,1.0);
    }
    return vec4(background,1);
    /* return vec4(object.z/MAX_DIST,0,0,1); */
}

void main()
{
    vec2 uv = (2.0*gl_FragCoord.xy - resolution.xy) / resolution.x;

    fragColor = render(uv);
}
