#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include "icamera.hpp"

namespace mhe
{
	class Camera : public iCamera
	{
		private:
			matrixf proj_m;
			// modelview matrix keep in Transform class
			v3d pos_;
			v3d dir_;

			cmn::uint id;
			
			void set_lookAt();
		public:
			Camera();	// with default settings
			
			void setPosition(const v3d& pos);			
			
			const v3d& getPosition() const
			{
				return pos_;
			}
			
			void setDirection(const v3d& dir);
			
			const v3d& getDirection() const
			{
				return dir_;
			}			
			
			void setPerspective(float fov, float aspect,
								float znear, float zfar);
										
			void update(const boost::shared_ptr<iDriver>& driver);

			void set_id(cmn::uint value)
			{
				id = value;
			}

			cmn::uint get_id() const
			{
				return id;
			}
	};
}

#endif
