[version 430]

[include "posteffect_vs_common.h"]

[fragment]

[include "common.h"]
[include "color_common.h"]
[include "posteffect_shared.h"]

[sampler2D src 0]

in VSOutput vsoutput;

out vec4 out_color;

void main()
{
    float key, threshold, intensity;
    unpack_bloom_settings(key, threshold, intensity);
    vec4 c = texture(src, vsoutput.tex);
    out_color = vec4(exposed_color(c.rgb, key, average_luminance, threshold) * intensity, c.a);
}
