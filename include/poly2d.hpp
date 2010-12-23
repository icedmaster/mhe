#ifndef _POLY2D_HPP_
#define _POLY2D_HPP_

#include "vector2d.hpp"
#include "base_types.h"

namespace mhe
{
    class poly2d
    {
        protected:
            v2d v[3];
        public:
            poly2d()
            {}
            
            poly2d(v2d v1, v2d v2, v2d v3)
            {
                v[0] = v1; v[1] = v2; v[2] = v3;
            }
            
            poly2d(v2d *vv)
            {
                v[0] = vv[0]; v[1] = vv[1]; v[2] = vv[2];
            }
            
            ~poly2d()
            {}
            
            v2d operator[] (uint n) const
            {
                return v[n];
            }            
    };
};

#endif
