#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
flat in vec3 COLOR;
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D heightMapSampler;
uniform sampler2D albedoSampler;
uniform vec3 sunPosition;

float heightScale = 1;

float get_height(vec2 uv)
{
    if (uv.x > 1 || uv.x < 0)
        return 0;
    if (uv.y > 1 || uv.y < 0)
        return 0;

    return heightScale*texture(heightMapSampler, uv).r;
}

vec3 get_normal(vec2 uv)
{
    const float step = 1.0f/1.0f;
    vec3 vertex = vec3(uv.x, 0, uv.y);
    vec3 UP    = vec3(uv.x, 0, uv.y+step);
    vec3 RIGHT = vec3(uv.x+step, 0, uv.y);

    vertex.y = get_height(vertex.xz);
    UP.y = get_height(UP.xz);
    RIGHT.y = get_height(RIGHT.xz);

    return normalize(cross(RIGHT-vertex,UP-vertex));
}

void main(){
	// Output color = color of the texture at the specified UV
    /* color = texture(albedoSampler, UV).rgb; */
    vec3 _color;
    _color = COLOR;

    float waterLevel = 0.4;
    float height = get_height(UV);
    if(height <= waterLevel) {
        /* color = vec3(0,0,0.5); */
        _color = mix(vec3(0,0,0.5), _color, exp(-1.2*waterLevel/height));
    }
    else {
        vec3 POS = vec3(UV.x, height, UV.y);
        vec3 dir = normalize(sunPosition - POS);

        float step = 0.001;
        for (int i = 10; i < 100; i++)
        {
            vec3 pos = POS + (dir*i*step);

            if(get_height(pos.xz) > pos.y)
            {
                _color = _color * (1.0-((100-i)/150.0));
                break;
            }
        }
    }

    /* color *= max(dot(dir, get_normal(UV)), 0.5); */
    /* color *= max(dot(dir, -get_normal(UV)), 0.9); */
    color = vec4(_color,1);
}
