[include "posteffect_vs_common.h"]

[fragment]

[include "color_common.h"]

[sampler2D input 0]

in VSOutput vsoutput;

out float color;

void main()
{
    vec4 c = texture(input, vsoutput.tex);
    color = calculate_luminance(c.rgb);
}
