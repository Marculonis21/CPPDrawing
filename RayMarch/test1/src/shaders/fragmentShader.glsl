#version 330 core

in vec4 gl_FragCoord;
out vec4 frag_color;

vec4 return_color()
{
    return vec4(0.0,0.0,1.0,1.0);
}
 
void main()
{
    frag_color = return_color();
}
