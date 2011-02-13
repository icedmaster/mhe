#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include "icamera.hpp"

namespace mhe
{
    class Camera : public iCamera
    {
        private:
            v3d pos;
            v3d dir;
            float x_ang, y_ang, z_ang;
            std::string name_;
            float z_near, z_far;
            float fov;

            void set_position(const v3d& v)
            {
                pos = v;
            }

            const v3d& get_position() const
            {
                return pos;
            }

            void set_direction(const v3d& d)
            {
                dir = d;
            }

            const v3d& get_direction() const
            {
                return dir;
            }

            const std::string& get_name() const
            {
                return name_;
            }

            void set_projection(cmn::uint w, cmn::uint h);

            void rotate_impl(const v3d& v, float ang);
            void move_impl(const v3d& v, float dist);
            void update_impl();
        public:
            Camera() :
                x_ang(0),
                y_ang(0),
                z_ang(0),
                z_near(0.1),
                z_far(100),
                fov(30)
            {
            }

            Camera(const v3d& position, const v3d& direction) :
                pos(position),
                dir(direction),
                x_ang(0),
                y_ang(0),
                z_ang(0),
                z_near(0.1),
                z_far(100),
                fov(30)
            {
            }

            ~Camera() {}
    };
};

#endif
