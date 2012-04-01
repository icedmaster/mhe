#ifndef _ICAMERA_HPP_
#define _ICAMERA_HPP_

#include "mhe_math.hpp"
#include "video_driver.hpp"
#include "transform.hpp"

namespace mhe
{
	class iCamera : public Transform
	{
		public:
			virtual ~iCamera() {}

			virtual void setPosition(const v3d&) = 0;
			virtual const v3d& getPosition() const = 0;
			virtual void setDirection(const v3d&) = 0;
			virtual const v3d& getDirection() const = 0;

			virtual void setPerspective(float fov, float aspect,
										float znear, float zfar) = 0;

			virtual void update(boost::shared_ptr<Driver>) = 0;
			virtual void set_id(cmn::uint id) = 0;
			virtual cmn::uint get_id() const = 0;
	};
}

#endif
