#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#include "math/vector3.hpp"
#include "math/quat.hpp"
#include "math/matrix.hpp"
#include "core/vector.hpp"

namespace mhe {

struct Skeleton;

struct Frame
{
	float time;
	vec3 position;
	quatf rotation;
	vec3 scale;
};

struct PositionAnimationFrame
{
	float time;
	vec3 value;
};

struct RotationAnimationFrame
{
	float time;
	quatf value;
};

struct ScaleAnimationFrame
{
	float time;
	vec3 value;
};

struct NodeAnimationFrame
{
	size_t node_id;
	vector<PositionAnimationFrame> positions;
	vector<RotationAnimationFrame> rotations;
	vector<ScaleAnimationFrame> scalings;
};

struct AnimationData
{
	float fps;
	float duration;
	vector<NodeAnimationFrame> frames;
	int play_mode;
};

struct SkeletalAnimationInstance
{
};

class AnimationController
{
public:
	void set_animation(const AnimationData* data, const Skeleton& skeleton);

	vec3 lerp(const vec3& v1, const vec3& v2, float t) const
	{
		return mhe::lerp(v1, v2, t);
	}

	quatf lerp(const quatf& q1, const quatf& q2, float t) const
	{
		return slerp(q1, q2, t);
	}

	template <class T, class V>
	bool sample(T& res, const V& data, float time, float duration) const
	{
		size_t size = data.size();
		for (int j = size - 1; j >= 0; --j)
		{
			size_t curr = j;
			size_t next = (j + 1) % size;

			float curr_time = data[curr].time;
			float next_time = next != 0 ? data[next].time : duration + data[0].time;

			float start_time = j == 0 ? 0.0f : data[curr].time;
			float end_time = next == 0 ? duration : data[next].time;

			if (time >= start_time && time <= end_time)
			{
				float t = (time - curr_time) / (next_time - curr_time);
				res = lerp(data[curr].value, data[next].value, t);
				return true;
			}
		}
		return false;
	}

	void update(float dt);

	const mat4x4* matrices() const
	{
		return &transforms_[0];
	}
private:
	const AnimationData* data_;
	vector<mat4x4> transforms_;
	vector<mat4x4> bind_pose_transforms_;
	float time_;
};

}

#endif
