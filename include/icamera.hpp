#ifndef _ICAMERA_HPP_
#define _ICAMERA_HPP_

#include "geom.hpp"

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
            virtual void update_impl() = 0;
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

            void update()
            {
                update_impl();
            }
    };
};

#endif
