#version 430 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D screen;
layout(binding = 1) uniform sampler2D map;

in vec2 UVs;

void main()
{
    FragColor = texture(screen, UVs);
}
