#ifndef __LINEAR_ANIMATION_LIST_IMPL_HPP__
#define __LINEAR_ANIMATION_LIST_IMPL_HPP__

#include "linear_animation_list.hpp"

namespace mhe {

#define LINEAR_ANIMATION_CLASS_DEFINITION(ClassName, FieldType)	\
	class ClassName : public LinearAnimationList<FieldType>

#define DEFAULT_LINEAR_ANIMATION_IMPLEMENTATION(ClassName, FieldType, field_name) \
	public:																\
	ClassName(size_t index, bool repeat = false) :						\
	LinearAnimationList<FieldType>(index, repeat) {}					\
private:																\
void update_node_for_value(Node* node, const FieldType& value)			\
{																		\
	node->set_##field_name(value);										\
}																		\
																		\
FieldType get_current_node_value(Node* node) const						\
{																		\
	return node->field_name();											\
}

#define IMPLEMENT_LINEAR_ANIMATION(ClassName, FieldType, field_name)	\
	LINEAR_ANIMATION_CLASS_DEFINITION(ClassName, FieldType)				\
	{																	\
		DEFAULT_LINEAR_ANIMATION_IMPLEMENTATION(ClassName, FieldType, field_name) \
	};

// for color animation
IMPLEMENT_LINEAR_ANIMATION(ColorLinearAnimationList, colorf, color);

// transform animation
class TransformLinearAnimationList : public LinearAnimationList<matrixf>
{
public:
	TransformLinearAnimationList(size_t index, bool repeat = false) :
		LinearAnimationList<matrixf>(index, repeat),
		apply_transform_(false)
	{}

	void set_translate_animation(const v3d& translate,
								 cmn::uint animation_part_time, cmn::uint total_animation_time)
	{
		apply_transform_ = true;
		cmn::uint count = total_animation_time / animation_part_time;
		const v3d& translate_part = translate / count;
		set_animation(matrixf::translation_matrix(translate_part),
					  animation_part_time, count);
	}

	void set_scale_animation(const v3d& scale,
							 cmn::uint animation_part_time, cmn::uint total_animation_time)
	{
		apply_transform_ = true;
		set_range(matrixf::empty_matrix(), matrixf::scaling_matrix(scale),
				  animation_part_time, total_animation_time);
	}

	void set_rotate_x_animation(float degrees,
								cmn::uint animation_part_time, cmn::uint total_animation_time)
	{
		apply_transform_ = true;
		set_range(matrixf::empty_matrix(), matrixf::rotation_matrix(degrees, 0, 0),
				  animation_part_time, total_animation_time);
	}

	void set_rotate_y_animation(float degrees,
								cmn::uint animation_part_time, cmn::uint total_animation_time)
	{
		apply_transform_ = true;
		set_range(matrixf::identity(), matrixf::rotation_matrix(0, degrees, 0),
				  animation_part_time, total_animation_time);
	}

	void set_rotate_z_animation(float degrees,
								cmn::uint animation_part_time, cmn::uint total_animation_time)
	{
		apply_transform_ = true;
		set_range(matrixf::identity(), matrixf::rotation_matrix(0, 0, degrees),
				  animation_part_time, total_animation_time);
	}
private:
	void update_node_for_value(Node* node, const matrixf& value)
	{
		if (apply_transform_)
			node->apply_transform(value);
		else node->set_transform(value);
	}

	matrixf get_current_node_value(Node* node) const
	{
		apply_transform_ = false;
		return node->transform();
	}

	void update_current_value(matrixf& current, const matrixf& delta) const
	{
		if (apply_transform_)
			current = delta;
		else current += delta;
	}
private:
	mutable bool apply_transform_;
};

}

#endif
