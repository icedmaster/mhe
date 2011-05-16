#include "mesh.hpp"

namespace mhe
{
    Mesh::Mesh()
    {
        v_.reserve(100);
        n_.reserve(100);
        t_.reserve(100);
        i_.reserve(100);
    }

    void Mesh::draw_impl(const boost::shared_ptr<iDriver>& driver)
    {
        driver->draw(v_, n_, t_, i_, tex_);
        // draw submeshes
        for (size_t i = 0; i < submesh.size(); ++i)
            submesh[i]->draw();
    }
};
