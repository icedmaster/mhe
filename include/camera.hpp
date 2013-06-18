#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <boost/shared_ptr.hpp>
#include "transform.hpp"

namespace mhe {

class Driver;

class Camera : public Transform
{
public:
	Camera();
	Camera(const vector3<float>& position, const vector3<float>& direction,
		   const vector3<float>& up);
	virtual ~Camera() {}

	void set(const vector3<float>& position, const vector3<float>& direction,
			 const vector3<float>& up);
	
	void set_projection(const matrixf& projection)
	{
		projection_ = projection;
	}

	const matrixf& projection() const
	{
		return projection_;
	}

	void update(Driver& driver);

	void enable_each_frame_updating(bool enable)
	{
		update_each_frame_ = enable;
		set_dirty();
	}

	void set_need_update()
	{
		updated_ = false;
	}

	void set_name(const std::string& name)
	{
		name_ = name;
	}

	const std::string& name() const
	{
		return name_;
	}
protected:
	void set_dirty()
	{
		updated_ = false;
	}

	virtual void update_view_matrix();
	virtual void update_projection(Driver& driver);
private:
	matrixf projection_;
	matrixf look_;
	std::string name_;
	bool update_each_frame_;
	bool updated_;
};

}

#endif
