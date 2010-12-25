#ifndef _CUBE_HPP_
#define _CUBE_HPP_

#include "geom.hpp"
#include "irenderable.hpp"

namespace mhe
{
    
    class Cube : public iRenderable
    {
        private:
            v3d ll_;
            v3d rh_;
            poly4d faces[6];

            void createPolygons(); 

	    void draw_impl();

	    cmn::uint num_vertexes() const
	    {
		return 8;
	    }
        public:
            Cube() : ll_(0, 0, 0), rh_(1, 1, -1)
            {
                createPolygons();
            }
            
            Cube(const v3d& ll, const v3d& rh) : ll_(ll), rh_(rh)
            {
                createPolygons();
            }
    };
    
};

#endif
