#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "core/array.hpp"
#include "math/matrix.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "utils/pool_utils.hpp"
#include "texture.hpp"

namespace mhe {

struct LightDesc
{
	struct Spot
	{
		float angle;
		float attenuation_a;
		float attenuation_b;
		float angle_attenuation;

		float spot_shadowmap_unused1;
		float spot_shadowmap_unused2;
		float spot_shadowmap_projection_znear;
		float spot_shadowmap_projection_zfar;
	};

	struct Omni
	{
		float radius;
		float omni_attenuation;
		float omni_attenuation_unused;
		float omni_unused1;

		float omni_shadowmap_unused1;
		float omni_shadowmap_unused2;
		float omni_shadowmap_unused3;
		float omni_shadowmap_unused4;
	};

	struct Directional
	{
		float directional_unused1;
		float directional_unused2;
		float directional_unused3;
		float directional_unused4;

		float directional_shadowmap_projection_width;
		float directional_shadowmap_projection_height;
		float directional_shadowmap_projection_znear;
		float directional_shadowmap_projection_zfar;
	};

	union
	{
		Spot spot;
		Omni omni;
		Directional directional;
	};

	float shadowmap_bias;
	bool cast_shadows;
	bool auto_shadow_configuration; // currently works for directional lights only

    LightDesc() :
        shadowmap_bias(0.00125f),
        cast_shadows(false),
				auto_shadow_configuration(true)
	{}
};

struct ShadingSettings
{
	vec4 diffuse;
	vec4 specular;
	float intensity;
	float specular_intensity;

	ShadingSettings() : intensity(1.0f), specular_intensity(1.0f) {}
};

struct ShadowInfo
{
	TextureInstance shadowmap;
	array<mat4x4, max_shadowmap_cascades_number> lightvp;
	array<mat4x4, max_shadowmap_cascades_number> lightview;
	array<vec3, max_shadowmap_cascades_number> offset;
	array<vec3, max_shadowmap_cascades_number> scale;
	array<float, max_shadowmap_cascades_number> znear;
	array<float, max_shadowmap_cascades_number> zfar;
	size_t cascades_number;
};

class Light
{
public:
	enum
	{
		spot,
		omni,
		directional
	};

	static const size_t light_types_number = directional + 1;
public:
	Light() :
		type_(spot),
		shadow_info_(nullptr)
	{}

	Light(int type) :
		type_(type),
		shadow_info_(nullptr)
	{}

	void set_type(int type)
	{
		type_ = type;
	}

	int type() const
	{
		return type_;
	}

	ShadingSettings& shading()
	{
		return shading_;
	}

	const ShadingSettings& shading() const
	{
		return shading_;
	}

	LightDesc& desc()
	{
		return desc_;
	}

	const LightDesc& desc() const
	{
		return desc_;
	}

	float attenuation_a() const
	{
		switch (type_)
		{
		case spot:
			return desc_.spot.attenuation_a;
		case omni:
			return desc_.omni.omni_attenuation;
		default: return 0.0f;
		}
	}

	float attenuation_b() const
	{
		if (type_ == spot) return desc_.spot.attenuation_b;
		return 1.0f;
	}

	float angle() const
	{
		switch (type_)
		{
		case spot: return desc_.spot.angle * 0.5f;
		case omni: return desc_.omni.radius;
		default: return 0.0f;
		}
	}

	float spot_angle_coeff() const
	{
		ASSERT(type_ == spot, "Invalid type");
		return cos(desc_.spot.angle * 0.5f);
	}

	float angle_attenuation() const
	{
		if (type_ == spot)
			return desc_.spot.angle_attenuation;
		return 1.0f;
	}

	void set_shadow_info(const ShadowInfo* info)
	{
		shadow_info_ = info;
	}

	const ShadowInfo* shadow_info() const
	{
		return shadow_info_;
	}
private:
	ShadingSettings shading_;
	LightDesc desc_;
	int type_;
	const ShadowInfo* shadow_info_;
};

}

#endif
