struct VSOutput
{
    vec3 normal;
    vec3 flux;
};

[vertex]

[sampler2D normal_texture 0]
[sampler2D flux_texture 1]
[sampler2D depth_texture 2]

[uniform InjectionSettings 0 permodel]
uniform InjectionSettings
{
    vec4 injection_settings;
};

out VSOutput vsoutput;

void main()
{
    ivec2 texsize = textureSize(normal_texture, 0);
    int index = gl_VertexID;
    vec2 texcoord = vec2(index % texsize.x, index / texsize.x);
    vec3 normal = texture(normal_texture, texcoord).xyz;
    vec3 flux = texture(flux_texture, texcoord).rgb;

    vsoutput.normal = normal;
    vsoutput.flux = flux;
}

[fragment]

[include "sh.h"]

in VSOutput vsoutput;

layout (location = 0) out vec4 out_r;
layout (location = 1) out vec4 out_g;
layout (location = 2) out vec4 out_b;

void main()
{
    vec3 normal = vsoutput.normal;
    SH4 sh = sh_cosine_lobe_sh4(normal);
    
    RGBSH4 color = mul(sh, vsoutput.flux);
    out_r = color.rgb[0];
    out_g = color.rgb[1];
    out_b = color.rgb[2];
}
