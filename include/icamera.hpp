#ifndef _ICAMERA_HPP_
#define _ICAMERA_HPP_

#include "mhe_math.hpp"
#include "idriver.hpp"
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

			virtual void update(const boost::shared_ptr<iDriver>&) = 0;
	};
}

#endif
