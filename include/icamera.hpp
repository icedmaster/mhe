#ifndef _ICAMERA_HPP_
#define _ICAMERA_HPP_

#include "geom.hpp"

namespace mhe
{
    class iCamera
    {
        public:
            //virtual ~iCamera() = 0;
            // accessors
            virtual void setPosition(const v3d& v) = 0;
            virtual const v3d& getPosition() const = 0;
            virtual void setDirection(const v3d& d) = 0;
            virtual const v3d& getDirection() const = 0;

            virtual void update() = 0;
    };
};

#endif
