#ifndef _ICAMERA_HPP_
#define _ICAMERA_HPP_

#include "types.hpp"
#include "geom.hpp"
#include <string>

namespace mhe
{
    class iCamera
    {
        private:
            virtual void set_position(const v3d& v) = 0;
            virtual const v3d& get_position() const = 0;
            virtual void set_direction(const v3d& d) = 0;
            virtual const v3d& get_direction() const = 0;

            virtual void rotate_impl(const v3d& v, float ang) = 0;
            virtual void move_impl(const v3d& v, float dist) = 0;
            virtual void update_impl() = 0;

            virtual const std::string& get_name() const = 0;
            virtual void set_projection(cmn::uint, cmn::uint) = 0;
        public:
            // accessors
            void setPosition(const v3d& v)
            {
                set_position(v);
            }

            const v3d& getPosition() const
            {
                return get_position();
            }

            void setDirection(const v3d& d)
            {
                set_direction(d);
            }

            const v3d& getDirection() const
            {
                return get_direction();
            }

            void rotate(const v3d& v, float ang)
            {
                rotate_impl(v, ang);
            }

            void move(const v3d& v, float dist)
            {
                move_impl(v, dist);
            }

            const std::string& name() const
            {
                return get_name();
            }

            void setProjection(cmn::uint w, cmn::uint h)
            {
                set_projection(w, h);
            }

            void update()
            {
                update_impl();
            }
    };
};

#endif
