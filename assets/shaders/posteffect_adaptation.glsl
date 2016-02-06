[include "posteffect_vs_common.h"]

[fragment]

[sampler2D dst 0]
[sampler2D curr 1]

[uniform AdaptationSettings 0 permodel]
uniform AdaptationSettings
{
    vec4 settings;
};

in VSOutput vsoutput;

out float out_color;

void main()
{
    float rate = settings.x;
    float delta = settings.y;
    float value = texture(dst, vsoutput.tex).x;
    float current = texture(curr, vsoutput.tex).x;

    out_color = current + (value - current) * delta / rate;
}

