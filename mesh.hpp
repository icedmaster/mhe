#ifndef _MESH_HPP_
#define _MESH_HPP_

#include "inode.hpp"
#include "types.hpp"
#include "itexture.hpp"
#include <vector>

namespace mhe
{
    class Mesh : public iNode
    {
        private:
            std::vector<float> v_;   // vertexes
            std::vector<float> n_;   // normals
            std::vector<float> t_;   // texture
            std::vector<cmn::uint> i_;   // indexes
            boost::shared_ptr<iTexture> tex_;

            std::vector< boost::shared_ptr<Mesh> > submesh;
        private:
            // implementations
            void update_impl(cmn::uint tick) {}
            void draw_impl(const boost::shared_ptr<iDriver>& driver);
        public:
            Mesh();

            void set(const std::vector<float>& v,
                     const std::vector<float>& n,
                     const std::vector<float>& t,
                     const std::vector<cmn::uint>& i)
            {
                v_ = v;
                n_ = n;
                t_ = t;
                i_ = i;
            }

            void setTexture(const boost::shared_ptr<iTexture>& t)
            {
                tex_ = t;
            }
    }
};

#endif
