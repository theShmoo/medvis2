
#version 110
//3x3 sobel filter

#extension GL_ARB_draw_buffers : enable

uniform sampler2D tex;


void main(void)
{
const SIZE = 9;
const vec2 KERNEL[SIZE] = {
    vec2(-1.0, 1.0), vec2(0.0, 1.0),
    vec2(1.0, 1.0), vec2(-1.0, 0.00),
    vec2(0.0, 0.0), vec2(1.0, 0.0),
    vec2(-1.0, -1.0), vec2(0.00, -1.0),
    vec2(1.0, -1.0),
  };

vec4 kernelValues[SIZE];

    for (int i = 0; i < SIZE; i++)
    {
        kernelValues[i] = texture2D(tex, 
                              gl_TexCoord[0].st + KERNEL[i]);
    }

//filter in both directions
    vec4 horizontalFilter = kernelValues[2] + (2.0*kernelValues[5]) + kernelValues[8] - (kernelValues[0] + (2.0*kernelValues[3]) + sample[6]);

    vec4 verticalFilter = kernelValues[0] + (2.0*kernelValues[1]) + kernelValues[2] - (kernelValues[6] + (2.0*kernelValues[7]) + kernelValues[8]);

//put all together
    //gl_FragColor.rgb = sqrt((horizEdge.rgb * horizEdge.rgb) + 
    //                        (vertEdge.rgb * vertEdge.rgb));
    //gl_FragColor.a = 1.0;
gl_FragData.rgb = sqrt((horizontalFilter.rgb * horizontalFilter.rgb) + (verticalFilter.rgb * verticalFilter.rgb));
gl_FragData.a = 1.0;

}
