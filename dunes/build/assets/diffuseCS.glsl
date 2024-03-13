#version 430 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

#define WIDTH 1024
#define HEIGHT 1024

#define DIFFUSE_SPEED 0.5f

vec4 surroundColorSum(ivec2 c)
{
    vec4 sum = vec4(0,0,0,0);

    for(int _y = -1; _y <= 1; _y++)
    {
        for(int _x = -1; _x <= 1; _x++)
        {
            ivec2 pos = c + ivec2(_x, _y);

            if(pos.x >= 0 && pos.x < WIDTH &&
               pos.y >= 0 && pos.y < HEIGHT)
            {
                sum += imageLoad(screen, pos);
            }
        }
    }

    sum = vec4(sum.x,sum.y,sum.z,1);
    return sum;
}

void main()
{
    uvec2 coords = gl_GlobalInvocationID.xy;

    vec4 sum = surroundColorSum(ivec2(coords));
    sum = sum / 9.0f;
    sum.w = 1;

    vec4 bluredValue = sum;
    vec4 orig = imageLoad(screen, ivec2(coords));

    vec4 color = mix(orig, bluredValue, DIFFUSE_SPEED);


    imageStore(screen, ivec2(coords), color);
}
