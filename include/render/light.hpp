#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "core/array.hpp"
#include "math/matrix.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "texture.hpp"
#include "res/serialize.hpp"
#include "res/serialize_basic_types.hpp"

namespace mhe {

enum LightType
{
    spot,
    omni,
    directional
};

namespace res
{
    struct Light : public Serializable
    {
        LightType type;
        float angle;
        float radius;
        float attenuation;
        float spot_attenuation;

        vec3 diffuse_color;
        vec3 specular_color;
        float intensity;
        float specular_intensity;

        float shadowmap_znear;
        float shadowmap_zfar;
        float shadowmap_bias;
        uint8_t cast_shadows;

        Light() :
            type(omni), angle(45.0f),
            radius(1.0f), attenuation(0.02f), spot_attenuation(0.02f),
            diffuse_color(color_rgb_white), specular_color(color_rgb_white),
            intensity(1.0f), specular_intensity(1.0f),
            shadowmap_znear(0.1f), shadowmap_zfar(100.0f),
            shadowmap_bias(0.00125f), cast_shadows(1)
        {}

        bool write(Serializer& serializer) const override
        {
            WRITE_ENUM(type, serializer);
            WRITE_FIELD(angle, serializer);
            WRITE_FIELD(radius, serializer);
            WRITE_FIELD(attenuation, serializer);
            WRITE_FIELD(spot_attenuation, serializer);
            WRITE_BASIC_TYPE_FIELD(diffuse_color, serializer);
            WRITE_BASIC_TYPE_FIELD(specular_color, serializer);
            WRITE_FIELD(intensity, serializer);
            WRITE_FIELD(specular_intensity, serializer);
            WRITE_FIELD(shadowmap_znear, serializer);
            WRITE_FIELD(shadowmap_zfar, serializer);
            WRITE_FIELD(shadowmap_bias, serializer);
            WRITE_FIELD(cast_shadows, serializer);
            return true;
        }

        bool read(Deserializer& serializer) override
        {
            TRY_READ_ENUM(type, serializer);
            TRY_READ_FIELD(angle, serializer);
            TRY_READ_FIELD(radius, serializer);
            TRY_READ_FIELD(attenuation, serializer);
            TRY_READ_FIELD(spot_attenuation, serializer);
            TRY_READ_BASIC_TYPE_FIELD(diffuse_color, serializer);
            TRY_READ_BASIC_TYPE_FIELD(specular_color, serializer);
            TRY_READ_FIELD(intensity, serializer);
            TRY_READ_FIELD(specular_intensity, serializer);
            TRY_READ_FIELD(shadowmap_znear, serializer);
            TRY_READ_FIELD(shadowmap_zfar, serializer);
            TRY_READ_FIELD(shadowmap_bias, serializer);
            TRY_READ_FIELD(cast_shadows, serializer);
            return true;
        }
    };

    // It's temporary here
    struct Transform : public Serializable
    {
        vec3 position;
        quatf rotation;
        vec3 scale;

        bool write(Serializer& serializer) const override
        {
            WRITE_BASIC_TYPE_FIELD(position, serializer);
            WRITE_BASIC_TYPE_FIELD(rotation, serializer);
            WRITE_BASIC_TYPE_FIELD(scale, serializer);
            return true;
        }

        bool read(Deserializer& serializer) override
        {
            TRY_READ_BASIC_TYPE_FIELD(position, serializer);
            TRY_READ_BASIC_TYPE_FIELD(rotation, serializer);
            TRY_READ_BASIC_TYPE_FIELD(scale, serializer);
            return true;
        }
    };
}

struct ShadowInfo
{
    TextureInstance shadowmap;
    array<mat4x4, max_shadowmap_cascades_number> lightvp;
    array<mat4x4, max_shadowmap_cascades_number> lightview;
    array<vec3, max_shadowmap_cascades_number> offset;
    array<vec3, max_shadowmap_cascades_number> scale;
    array<float, max_shadowmap_cascades_number> znear;
    array<float, max_shadowmap_cascades_number> zfar;
    array<vec4, max_shadowmap_cascades_number> scsm_params;
    size_t cascades_number;
};

}

#endif
