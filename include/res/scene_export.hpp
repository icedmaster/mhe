#ifndef __SCENE_EXPORT_HPP__
#define __SCENE_EXPORT_HPP__

#include "core/types.hpp"
#include "core/string.hpp"
#include "core/hashmap.hpp"
#include "math/vector3.hpp"
#include "math/aabb.hpp"
#include "math/matrix.hpp"
#include "math/quat.hpp"
#include "render/material.hpp" // MaterialRenderData

namespace mhe {

const char* const mhe_header = "mhe";

const char* const mhe_material_extension = ".mtl";

const size_t max_weights_per_bone = 4;

struct Weight
{
    uint32_t vertex_id;
    float weight;
};

struct PositionAnimationExportFrame
{
    float time;
    vec3 position;
};

struct RotationAnimationExportFrame
{
    float time;
    quatf rotation;
};

struct ScaleAnimationExportFrame
{
    float time;
    vec3 scale;
};

enum
{
    play_mode_clamp,
    play_mode_loop
};

struct AnimationFrame
{
    size_t node_id;
    float time;
    vec3 position;
    quatf rotation;
    vec3 scale;
};

struct NodeAnimationExportFrame
{
    size_t node_id;
    size_t position_frames_number;
    PositionAnimationExportFrame* position_frames;
    size_t rotation_frames_number;
    RotationAnimationExportFrame* rotation_frames;
    size_t scale_frames_number;
    ScaleAnimationExportFrame* scale_frames;
};

struct AnimationExportData
{
    string name;
    float fps;
    float duration;
    size_t frames_number;
    NodeAnimationExportFrame* frames;
    int play_mode;
};

struct MeshPartExportData
{
    uint32_t vbuffer_offset;
    uint32_t ibuffer_offset;
    uint32_t faces_number;
};

enum TraceDataType
{
    trace_data_grid = 0
};

template <class V, class I>
void calculate_tangents(V* vertices, size_t vertices_number, I* indices, size_t indices_number)
{
    vec3* tan1 = new vec3[vertices_number * 2];
    vec3* tan2 = tan1 + vertices_number;

    ::memset((void*)tan1, 0, vertices_number * 2 * sizeof(vec3));

    for (size_t a = 0; a < indices_number; a += 3)
    {
        I i1 = indices[a];
        I i2 = indices[a + 1];
        I i3 = indices[a + 2];

        const vec3& v1 = vertices[i1].pos;
        const vec3& v2 = vertices[i2].pos;
        const vec3& v3 = vertices[i3].pos;

        const vec2& w1 = vertices[i1].tex;
        const vec2& w2 = vertices[i2].tex;
        const vec2& w3 = vertices[i3].tex;

        float x1 = v2.x() - v1.x();
        float x2 = v3.x() - v1.x();
        float y1 = v2.y() - v1.y();
        float y2 = v3.y() - v1.y();
        float z1 = v2.z() - v1.z();
        float z2 = v3.z() - v1.z();

        float s1 = w2.x() - w1.x();
        float s2 = w3.x() - w1.x();
        float t1 = w2.y() - w1.y();
        float t2 = w3.y() - w1.y();

        float d = (s1 * t2 - s2 * t1);
        float r = d != 0.0f ? 1.0f / d : 0.0f;
        vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
            (t2 * z1 - t1 * z2) * r);
        vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
            (s1 * z2 - s2 * z1) * r);

        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    }

    for (size_t a = 0; a < vertices_number; ++a)
    {
        const vec3& n = vertices[a].nrm;
        const vec3& t = tan1[a];

        vec3 tng;
        // Gram-Schmidt orthogonalize
        tng = (t - n * dot(n, t)).normalized();

        // Calculate handedness
        float w = (dot(cross(n, t), tan2[a]) < 0.0f) ? -1.0f : 1.0f;
        vertices[a].tng = vec4(tng, w);
    }

    delete [] tan1;
}

}

#endif
