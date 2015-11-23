#include "render/animation.hpp"

#include "render/mesh.hpp"

namespace mhe {

void AnimationController::set_animation(const AnimationData* data, const Skeleton& skeleton)
{
	data_ = data;
	time_ = 0.0f;
	transforms_.resize(skeleton.bones.size());
	for (size_t i = 0, size = transforms_.size(); i < size; ++i)
		transforms_[i] = skeleton.bones[i].local_transform;
	bind_pose_transforms_ = transforms_;
}

void AnimationController::update(float dt)
{
	time_ += dt;
	if (time_ > data_->duration)
		time_ -= data_->duration;
	size_t size = data_->frames.size();
	for (size_t i = 0; i < size; ++i)
	{
		const NodeAnimationFrame& frame = data_->frames[i];

		vec3 position;
		quatf rotation;
		vec3 scale(1, 1, 1);
		bool found = sample(position, frame.positions, time_, data_->duration);
		found |= sample(rotation, frame.rotations, time_, data_->duration);
		found |= sample(scale, frame.scalings, time_, data_->duration);

		if (found)
		{
			mat4x4 m = mat4x4::scaling_matrix(scale);
			m *= rotation.to_matrix<mat4x4>();
			m.set_row(3, position);
			transforms_[frame.node_id] = m;
		}
		else
			transforms_[frame.node_id] = bind_pose_transforms_[frame.node_id];
	}
}

}
