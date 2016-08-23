[version 430]

[defs MODE 0 2]

#define CONSTANT_AMBIENT_MODE 0
#define SH_AMBIENT_MODE 1

[include "posteffect_vs_common.h"]

[fragment]

[include "common.h"]
[include "geometry_common.h"]
[include "sh.h"]
[include "gbuffer_pbr_common.h"]

in VSOutput vsoutput;

#if MODE == SH_AMBIENT_MODE
layout(std430, binding = 2) buffer SHData
{
    ColorSH9 ambient_sh;
};
#endif

out vec3 out_color;

void main()
{
#if MODE == CONSTANT_AMBIENT_MODE
    out_color = ambient.rgb;
#elif MODE == SH_AMBIENT_MODE
    vec3 nrm_ws = gbuffer_normal_ws(vsoutput.tex);
    out_color = calculate_irradiance(nrm_ws, ambient_sh);
#endif
}
