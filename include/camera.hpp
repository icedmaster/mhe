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
        public:
            Camera()
            {
            }

            Camera(const v3d& position, const v3d& direction) :
                pos(position),
                dir(direction)
            {
            }

            ~Camera() {}

            void setPosition(const v3d& p)
            {
                pos = p;
            }

            const v3d& getPosition() const
            {
                return pos;
            }

            void setDirection(const v3d& d)
            {
                dir = d;
            }

            const v3d& getDirection() const
            {
                return dir;
            }

            void update();
    };
};

#endif
