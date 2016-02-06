[include "posteffect_vs_common.h"]

[fragment]

[sampler2D src 0]

in VSOutput vsoutput;

out vec4 out_color;

void main()
{
    out_color = texture(src, vsoutput.tex);
}
