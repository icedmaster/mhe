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
        READ_FIELD(ref, deserializer);
        return true;
    }
};

struct Material : public Serializable
{
    string id;
    FilePath ref;
    Texture albedo;
    Texture normalmap;

    bool write(Serializer& serializer) const override
    {
        WRITE_FIELD(albedo, serializer);
        WRITE_FIELD(normalmap, serializer);
        return true;
    }

    bool read(Deserializer& deserializer) override
    {
        READ_FIELD(albedo, deserializer);
        READ_FIELD(normalmap, deserializer);
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
