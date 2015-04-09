#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

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

    LightDesc() :
        shadowmap_bias(0.00125f),
        cast_shadows(false)
	{}
};

struct ShadingSettings
{
	vec4 diffuse;
	vec4 specular;
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
		type_(spot)
	{}

	Light(int type) :
		type_(type)
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

	TextureInstance shadowmap_texture() const
	{
		return shadowmap_texture_;
	}

	void set_shadowmap_texture(const TextureInstance& texture)
	{
		shadowmap_texture_ = texture;
	}
private:
	ShadingSettings shading_;
	LightDesc desc_;
	int type_;

	TextureInstance shadowmap_texture_;
};

}

#endif
