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
	virtual ~Camera() {}
	void update(const boost::shared_ptr<Driver>& driver);

	void enable_each_frame_updating(bool enable)
	{
		update_each_frame_ = enable;
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
private:
	bool update_each_frame_;
	bool updated_;
	std::string name_;
};

}

#endif
