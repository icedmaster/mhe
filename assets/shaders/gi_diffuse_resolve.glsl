[version 430]

[defs MODE 0 2]
[defs GI_TEXTURE 0 1]

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
    float ambient_sh_data[27];
};
#endif

[sampler2D gi_texture 4]

out vec3 out_color;

void main()
{
#if MODE == CONSTANT_AMBIENT_MODE
    out_color = ambient.rgb;
#elif MODE == SH_AMBIENT_MODE
    vec3 nrm_ws = gbuffer_normal_ws(vsoutput.tex);
    out_color = calculate_irradiance(nrm_ws, read_sh9(ambient_sh_data));
#endif

#if GI_TEXTURE == 1
    out_color = out_color + texture(gi_texture, vsoutput.tex).rgb;
#endif
}
