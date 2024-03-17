#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 POS;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform sampler2D albedoSampler;

float get_height(vec2 uv)
{
    if (uv.x > 1 || uv.x < 0)
        return 0;
    if (uv.y > 1 || uv.y < 0)
        return 0;

    return texture(heightMapSampler, uv).r;
}

const vec3 sunPosition = vec3(0,1,0);
void main(){

	// Output color = color of the texture at the specified UV
    color = texture(albedoSampler, UV).rgb;
    
    vec3 dir = normalize(sunPosition - POS);
    float step = 0.05;
    for (int i = 2; i < 100; i++)
    {
        vec3 pos = POS + (dir*i*step);

        if(get_height(pos.xz/10.0) > pos.y)
        {
            color = color * (1.0-((100-i)/150.0));
            break;
        }
    }
}
