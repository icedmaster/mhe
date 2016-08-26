#ifndef __MHE_BINARY_MESH_HPP__
#define __MHE_BINARY_MESH_HPP__

#include <fstream>
#include "core/vector.hpp"
#include "math/aabb.hpp"
#include "serialize.hpp"
#include "serialize_basic_types.hpp"

namespace mhe {

struct Mesh;
struct Context;

namespace res {

// TODO: maybe move next structs to the separate header?
struct Texture : public Serializable
{
    FilePath ref;

    bool write(Serializer& serializer) const override
    {
        WRITE_FIELD(ref, serializer);
        return true;
    }

    bool read(Deserializer& deserializer) override
    {
        READ_FIELD_WITH_DEFAULT(ref, deserializer, FilePath());
        return true;
    }
};

struct Material : public Serializable
{
    string id;
    FilePath ref;
    Texture albedo;
    Texture normalmap;
    Texture roughness_map;
    Texture metalness_map;

    bool write(Serializer& serializer) const override
    {
        WRITE_FIELD(albedo, serializer);
        WRITE_FIELD(normalmap, serializer);
        WRITE_FIELD(roughness_map, serializer);
        WRITE_FIELD(metalness_map, serializer);
        return true;
    }

    bool read(Deserializer& deserializer) override
    {
        READ_FIELD(albedo, deserializer);
        READ_FIELD(normalmap, deserializer);
        READ_FIELD(roughness_map, deserializer);
        READ_FIELD(metalness_map, deserializer);
        return true;
    }
};

struct Mesh : public Serializable
{
    FilePath binary;
    AABBf aabb;

    struct Part : public Serializable
    {
        AABBf aabb;
        FilePath material;

        bool write(Serializer& serializer) const override
        {
            WRITE_FIELD(material, serializer);
            return true;
        }

        bool read(Deserializer& deserializer) override
        {
            READ_FIELD(material, deserializer);
            return true;
        }
    };

    vector<Part> parts;

    bool write(Serializer& serializer) const override
    {
        WRITE_FIELD(binary, serializer);
        WRITE_BASIC_TYPE_FIELD(aabb, serializer);
        WRITE_FIELD(parts, serializer);
        return true;
    }

    bool read(Deserializer& deserializer) override
    {
        READ_FIELD(binary, deserializer);
        READ_BASIC_TYPE_FIELD(aabb, deserializer);
        READ_FIELD(parts, deserializer);
        return true;
    }
};
}

bool mhe_binary_mesh_load(Mesh& mesh, const FilePath& filename, Context* context);

}

#endif
