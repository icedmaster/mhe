[version 430]
[defs LIGHT_TYPE 0 2]
[defs SHADOWMAP 0 1]
[defs SHADOWMAP_QUALITY 0 2]

#define LIGHT_DIRECTION_FROM_SOURCE
#define CASCADED_SHADOWMAP

[include "common.h"]
[include "lighting_common.h"]

#ifdef DIRECTIONAL_CSM
//#define CASCADED_SHADOWMAP_DEBUG
#endif

// Define layout before "geometry_common.h" will be included
// TODO: SPOT_LIGHT is temporary here
#if LIGHT_TYPE == DIRECTIONAL_LIGHT || LIGHT_TYPE == SPOT_LIGHT
#define FULLSCREEN_LAYOUT
#else
#define DEBUG_LAYOUT
#endif

[include "geometry_common.h"]

[uniform lights 2 permodel]
uniform lights
{
	Light light;
};

#ifndef FULLSCREEN_LAYOUT
struct VSOutput
{
	vec4 pos;
};
#else
struct VSOutput
{
	vec2 tex;
};
#endif

[vertex]

out VSOutput vsoutput;

#ifndef FULLSCREEN_LAYOUT
void main()
{
	vec4 out_pos = vp * light.lightw * vec4(pos, 1.0f);
	vsoutput.pos = out_pos;
	gl_Position = out_pos;
}
#else
void main()
{
	vsoutput.tex = tex;
	gl_Position = pos;
}
#endif

[fragment]

[include "gbuffer_pbr_common.h"]
[include "brdf.h"]
[include "shadow_common.h"]

in VSOutput vsoutput;

out vec3 out_color;

void main()
{
	// TODO: add pack and unpack methods
#ifndef FULLSCREEN_LAYOUT
	vec2 tex = vsoutput.pos.xy / vsoutput.pos.w * 0.5f + 0.5f;
#else
	vec2 tex = vsoutput.tex;
#endif
    GBuffer gbuffer = gbuffer_unpack(tex);
    if (gbuffer.depth > 0.9999f)
    {
        out_color = VEC3_ZERO;
        return;
    }

	vec3 pos_ws = position_from_depth(tex, gbuffer.depth, inv_vp);
	vec3 V = normalize(viewpos.xyz - pos_ws);
    vec3 L = light_direction(pos_ws, light);
    vec3 N = gbuffer.normal;

    vec3 H = normalize(V + L);
    
    float ndotl = saturate(dot(N, L));
    float ndotv = abs(dot(N, V));
    float ldoth = saturate(dot(L, H));
    float ndoth = saturate(dot(N, H));

    IndirectLighting indirect_lighting;
    indirect_lighting.diffuse = ambient.rgb;
    indirect_lighting.specular = VEC3_ZERO;

    float shadow = get_shadow_value(vec3(tex, gbuffer.depth), pos_ws, linearized_depth(gbuffer.depth, znear, zfar), light);
	vec3 result = BRDF_lambert_ggx(gbuffer, indirect_lighting, light.diffuse.rgb * shadow, light.specular.rgb * shadow, ndotl, ndotv, ndoth, ldoth);

	out_color = result;
}
