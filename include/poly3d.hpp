#ifndef _POLY3D_HPP_
#define _POLY3D_HPP_

#include "vector3d.hpp"

namespace mhe
{
    
    class poly3d
    {
        private:
            v3d v[3];
            v3d n;   // normal
            
            void calcNormals()
            {
                v3d v1 = v[0] - v[1];
                v3d v2 = v[2] - v[1];
                n = cross(v1, v2);
                n.normalize();                                                                                
            }                    
    
        public:
            poly3d()
            {}
            
            poly3d(const v3d& v1, const v3d& v2, const v3d& v3) 
            {
                set(v1, v2, v3);    
            }
            
            void set(const v3d& v1, const v3d& v2, const v3d& v3)
            {
                v[0] = v1; v[1] = v2; v[2] = v3;
                calcNormals();
            }
            
            const v3d& operator[] (uint pos) const
            {
                return getVertex(pos);
            }
            
            const v3d& getVertex(uint pos) const
            {
                if (pos > 2) return v[0];
                return v[pos];
            }
            
            const v3d& getNormal() const
            {               
				return n;
	        }            
    };
    
};

#endif
            
