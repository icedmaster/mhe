[version 430]

struct VSOutput
{
    vec3 pos;
    vec3 nrm;
    vec2 tex;
};

[uniform Settings 0 percamera]
layout (binding = 0) uniform Settings
{
    vec4 grid_size;
    mat4 vp[3];
    mat4 worldspace_to_voxelspace;
};

[vertex]

#define CUSTOM_PERCAMERA_UNIFORM
[include "geometry_common.h"]

out VSOutput vsoutput;

void main()
{
    vsoutput.pos = (model * vec4(pos, 1.0f)).xyz;
    vsoutput.nrm = (normal * vec4(nrm, 0.0f)).xyz;
    vsoutput.tex = tex;
}

[geometry]

in VSOutput vsoutput[3];
out VSOutput gsoutput;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

void main()
{
    vec3 triangle_normal = normalize(vsoutput[0].nrm + vsoutput[1].nrm + vsoutput[2].nrm);

    vec3 axes[3] = vec3[3]( vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) );
    float max_len = 0.0f;
    int main_axis = 0;
    for (int i = 0; i < 3; ++i)
    {
        float proj_len = abs(dot(axes[i], triangle_normal));
        if (proj_len > max_len)
        {
            max_len = proj_len;
            main_axis = i;
        }
    }

    vec3 pos_middle = (vsoutput[0].pos + vsoutput[1].pos + vsoutput[2].pos) / 3.0f;
    vec3 pos_middle_cs = (vp[main_axis] * vec4(pos_middle, 1.0f)).xyz;

    float rasterization_offset = grid_size.w;

    for (int i = 0; i < 3; ++i)
    {
        vec4 proj_pos = vp[main_axis] * vec4(vsoutput[i].pos, 1.0f);
        proj_pos.xyz += normalize(proj_pos.xyz - pos_middle_cs) * rasterization_offset;
        gsoutput.pos = (worldspace_to_voxelspace * vec4(vsoutput[i].pos, 1.0f)).xyz;
        gsoutput.nrm = vsoutput[i].nrm;
        gsoutput.tex = vsoutput[i].tex;
        gl_Position = proj_pos;
        EmitVertex();
    }
    EndPrimitive();
}

[fragment]

in VSOutput gsoutput;

layout (binding = 0) uniform sampler2D albedo_texture;

layout (r32ui, binding = 1) uniform uimageBuffer position_buffer; 
layout (r32ui, binding = 2) uniform volatile uimage3D color_image;
layout (r32ui, binding = 3) uniform volatile uimage3D normal_image;

layout (binding = 0) uniform atomic_uint voxel_index;

uint uvec3_to_uint(uvec3 v)
{
    return ((uint(v.z) << 20) & 0x03ff0000) | ((uint(v.y) << 10) & 0x0003ff00) | (uint(v.x) & 0x3ff);
}

uint vec4_to_uint(vec4 v)
{
    return ((uint(v.w) << 24) & 0xff000000) | ((uint(v.z) << 16) & 0x00ff0000) | ((uint(v.y) << 8) & 0x0000ff00) | (uint(v.x) & 0xff);
}

vec4 uint_to_vec4(uint v)
{
    float x = float(v & 0xff);
    float y = float((v >> 8) & 0xff);
    float z = float((v >> 16) & 0xff);
    float w = float((v >> 24) & 0xff);
    return vec4(x, y, z, w);
}

void calculate_average_and_save(layout (r32ui) volatile uimage3D out_image, ivec3 coord, vec4 value)
{
    value *= 255.0f;
    imageStore(out_image, coord, uvec4(vec4_to_uint(value)));
    return;

    uint prev = 0;
    uint counter = 0;
    value.w = 1.0f;
    value.xyz *= 255.0f;
    uint curr = vec4_to_uint(value);
    uint original = 0;
    //  gint imageAtomicCompSwap(gimage image​, IMAGE_COORDS, gint compare​, gint data​)
    // imageAtomicCompSwap doesn't write the data until the current value is equal to compare.
    // The method returns the original value and if we want to know whether it's been modified we need to
    // test it against the "compare" parameter
    while ((original = imageAtomicCompSwap(out_image, coord, prev, curr)) != prev && counter < 100) // until we really write the data (but other threads change it)
    {
        prev = original; // the value has changed
        vec4 new_value = uint_to_vec4(original);
        new_value.xyz *= new_value.w;
        new_value += value;
        new_value.xyz /= new_value.w;
        curr = vec4_to_uint(new_value);
        ++counter;
    }

    // now we got the averaged value
    value = uint_to_vec4(curr);
    value.w = 255.0f;
    imageStore(out_image, coord, uvec4(vec4_to_uint(value)));
}

void main()
{
    uvec3 voxel_pos = uvec3(floor(gsoutput.pos));

    vec4 c = texture(albedo_texture, gsoutput.tex);

    uint index = atomicCounterIncrement(voxel_index);
    memoryBarrier();

    imageStore(position_buffer, int(index), uvec4(uvec3_to_uint(voxel_pos)));

    calculate_average_and_save(color_image, ivec3(voxel_pos), c);
}
