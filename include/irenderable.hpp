#ifndef _IRENDERABLE_HPP_
#define _IRENDERABLE_HPP_

#include "geom.hpp"
#include "types.hpp"
#include "mhe_gl.hpp"

namespace mhe
{
    class iRenderable
    {
        private:
            virtual void draw_impl() = 0;

            virtual cmn::uint num_vertexes() const = 0;

            virtual int get_id() const
            {
                return 0;
            }

            /*virtual const std::string& get_name() const
            {
                return "";
            }*/
        public:
            void draw()
            {
                draw_impl();
            }

            cmn::uint numVertexes() const
            {
                return num_vertexes();
            }

            int id() const
            {
                return get_id();
            }

            /*const std::string& name() const
            {
                return get_name();
            }*/
    };

    // grafic primitives
    // polygon
    class poly3d : public iRenderable
    {
        private:
            v3d v[3];
            v3d n;      // normal

            void calc_normal()
            {
                v3d v1 = (v[1] - v[0]);
                v3d v2 = (v[2] - v[0]);
                n = cross(v1, v2);
                n.normalize();
            }

            void draw_impl()
            {
                glBegin(GL_POLYGON);
                glNormal3fv(n.get());
                glVertex3fv(v[0].get());
                glVertex3fv(v[1].get());
                glVertex3fv(v[2].get());
                glEnd();
            }

            cmn::uint num_vertexes() const
            {
                return 3;
            }
        public:
            poly3d()
            {
            }

            poly3d(const v3d& v0, const v3d& v1, const v3d& v2)
            {
                v[0] = v0; v[1] = v1; v[2] = v2;
                calc_normal();
            }

            void invertNormal()
            {
                n.set(-n.x(), -n.y(), -n.z());
            }
    };

    class poly4d : public iRenderable
    {
        private:
            v3d v[4];
            v3d n;      // normal

            void calc_normal()
            {
                v3d v1 = (v[2] - v[1]);
                v3d v2 = (v[1] - v[0]);
                n = cross(v1, v2);
                n.normalize();
            }

            void draw_impl()
            {
                glBegin(GL_QUADS);
                glNormal3fv(n.get());
                glVertex3fv(v[0].get());
                glVertex3fv(v[1].get());
                glVertex3fv(v[2].get());
                glVertex3fv(v[3].get());
                glEnd();
            }

            cmn::uint num_vertexes() const
            {
                return 3;
            }
        public:
            poly4d()
            {
            }

            poly4d(const v3d& v0, const v3d& v1, const v3d& v2, const v3d& v3)
            {
                set(v0, v1, v2, v3);
            }

            const v3d& getNormal() const
            {
                return n;
            }

            void invertNormal()
            {
                n.set(-n.x(), -n.y(), -n.z());
            }

            void set(const v3d& v0, const v3d& v1, const v3d& v2, const v3d& v3)
            {
                v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3;
                calc_normal();
            }
    };
};

#endif
