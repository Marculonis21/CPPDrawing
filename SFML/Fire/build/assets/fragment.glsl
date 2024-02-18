#version 430 core

uniform sampler2D texture;
uniform float xResolution;
uniform float yResolution;

in vec4 gl_TexCoord;

void main()
{
    vec2 offx = vec2(xResolution, 0.0);
    vec2 offy = vec2(0.0, yResolution);

        float weightCentre = 1.0;//TODO: Could it be an issue of float precision when numbers are summed together and then divided at the end?
        float weightAdj    = 0.5;
        float weightDiag   = 0.25;

        float weights[3] = {weightCentre, weightAdj, weightDiag};

        //Alpha weighted colour blending: (maybe there is some built in way to achieve what I'm doing here trivially)
        float alphaWeight = 0.0;
        float colourWeight = 0.0;

        vec4 colour = vec4(0.0, 0.0, 0.0, 0.0);

        for(int dx = -1; dx <= 1; ++dx) {
                for(int dy = -1; dy <= 1; ++dy) {
                        
                        vec4 texColour = texture2D(texture, gl_TexCoord[0].xy + float(dx) * offx + float(dy) * offy);

                        int ds = dx*dx + dy*dy;//This works nicely because numbers have magnitude 0 or 1, but won't stretch further
                        float weight = weights[ds];

                        float effectiveWeight = texColour.a * weight;

                        colour.a += effectiveWeight;
                        alphaWeight += weight;

                        colour.rgb += texColour.rgb * effectiveWeight;
                        colourWeight += effectiveWeight;

                }
        }
        
        //Make certain we never divide by zero
        colour.a = alphaWeight > 0.0 ? colour.a / alphaWeight : 0.0;
        colour.rgb = colourWeight > 0.0 ? colour.rgb / colourWeight : colour.rgb;

    gl_FragColor = colour;
}
