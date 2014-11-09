#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "utils/pool_utils.hpp"

namespace mhe {

struct LightDesc
{
	struct Spot
	{
		float angle;
		float attenuation;
		float angle_attenuation;
	};

	struct Omni
	{
		float radius;
		float omni_attenuation;
		float omni_unused1;
	};

	struct Directional
	{
		float directional_unused1;
		float directional_unused2;
		float directional_unused3;
	};

	union
	{
		Spot spot;
		Omni omni;
		Directional directional;
	};

	bool cast_shadows;
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

	float attenuation() const
	{
		switch (type_)
		{
		case spot:
			return desc_.spot.attenuation;
		case omni:
			return desc_.omni.omni_attenuation;
		default: return 1.0f;
		}
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
private:
	ShadingSettings shading_;
	LightDesc desc_;
	int type_;
};

}

#endif
