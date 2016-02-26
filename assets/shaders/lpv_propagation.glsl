[defs OCCLUSION 0 1]

[uniform InjectionSettings 0 permodel]
uniform InjectionSettings
{
    vec4 injection_settings;
    mat4 rsm_to_world;
    mat4 world_to_lpv;
    vec4 light_parameters;
};

struct VSOutput
{
    ivec3 pos;
};

[vertex]

flat out VSOutput vsoutput;

void main()
{
    int cell_size = int(injection_settings.x);
    int index = gl_VertexID;
    int z = index / (cell_size * cell_size);
    int y = (index % (cell_size * cell_size)) / cell_size;
    int x = index % cell_size;
    vsoutput.pos = ivec3(x, y, z);

    gl_Position = vec4(vec2(vsoutput.pos.xy) / float(cell_size) * 2.0f - 1.0f, 0, 1);
}

[geometry]

flat in VSOutput vsoutput[];

flat out VSOutput gsoutput;

layout(points) in;
layout(points, max_vertices = 1) out;

void main()
{
    gl_Layer = vsoutput[0].pos.z;
    gsoutput = vsoutput[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
}

[fragment]

[include "sh.h"]

[sampler3D sh_r_texture 0]
[sampler3D sh_g_texture 1]
[sampler3D sh_b_texture 2]

#if OCCLUSION == 1
[sampler3D sh_occlusion_texture 3]
#endif

layout (location = 0) out vec4 out_r;
layout (location = 1) out vec4 out_g;
layout (location = 2) out vec4 out_b;

flat in VSOutput gsoutput;

RGBSH4 fetch_data(ivec3 pos)
{
    RGBSH4 res;
    res.rgb[0] = texelFetch(sh_r_texture, pos, 0);
    res.rgb[1] = texelFetch(sh_g_texture, pos, 0);
    res.rgb[2] = texelFetch(sh_b_texture, pos, 0);
    return res;
}

RGBSH4 propagate_dir(ivec3 cell_pos, ivec3 dir)
{
    RGBSH4 adjacent_cell_stored_radiance = fetch_data(cell_pos + dir);
    // calculate incoming radiance
    vec3 normal = dir;
    vec3 irradiance = max(calculate_irradiance(-normal, adjacent_cell_stored_radiance), 0.0f);
#if OCCLUSION == 1
    vec4 stored_occlusion_sh = texelFetch(sh_occlusion_texture, cell_pos, 0);
    SH4 occlusion_sh_coeff = sh_cosine_lobe_sh4(normal);
    float occlusion = dot(stored_occlusion_sh, occlusion_sh_coeff.c);
    irradiance *= saturate(1.0f - occlusion);
#endif
    // back to SH space using the direction of propagation
    SH4 sh_coeff = sh_cosine_lobe_sh4(-normal);
    return mul(sh_coeff, irradiance);
}

void main()
{
    RGBSH4 res = propagate_dir(gsoutput.pos, ivec3(-1, 0, 0));
    res = add(res, propagate_dir(gsoutput.pos, ivec3(1, 0, 0)));
    res = add(res, propagate_dir(gsoutput.pos, ivec3(0, -1, 0)));
    res = add(res, propagate_dir(gsoutput.pos, ivec3(0, 1, 0)));
    res = add(res, propagate_dir(gsoutput.pos, ivec3(0, 0, -1)));
    res = add(res, propagate_dir(gsoutput.pos, ivec3(0, 0, 1)));

    out_r = res.rgb[0];
    out_g = res.rgb[1];
    out_b = res.rgb[2];
}
