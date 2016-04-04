#ifndef __FRUSTUM_HPP__
#define __FRUSTUM_HPP__

#include "plane.hpp"
#include "matrix.hpp"
#include "aabb.hpp"

namespace mhe {

template <class T>
class frustum
{
public:
    enum
    {
        plane_left,
        plane_right,
        plane_bottom,
        plane_top,
        plane_near,
        plane_far
    };

    // http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
    template <class U>
    void set(const matrix<U>& m)
    {
        // left
        planes_[plane_left].set(m.element(0, 3) + m.element(0, 0),
                                m.element(1, 3) + m.element(1, 0),
                                m.element(2, 3) + m.element(2, 0),
                                m.element(3, 3) + m.element(3, 0));
        // right
        planes_[plane_right].set(m.element(0, 3) - m.element(0, 0),
                                 m.element(1, 3) - m.element(1, 0),
                                 m.element(2, 3) - m.element(2, 0),
                                 m.element(3, 3) - m.element(3, 0));
        // bottom
        planes_[plane_bottom].set(m.element(0, 3) + m.element(0, 1),
                                 m.element(1, 3) + m.element(1, 1),
                                 m.element(2, 3) + m.element(2, 1),
                                 m.element(3, 3) + m.element(3, 1));
        // top
        planes_[plane_top].set(m.element(0, 3) - m.element(0, 1),
                                 m.element(1, 3) - m.element(1, 1),
                                 m.element(2, 3) - m.element(2, 1),
                                 m.element(3, 3) - m.element(3, 1));
        // near
        planes_[plane_near].set(m.element(0, 3) + m.element(0, 2),
                                 m.element(1, 3) + m.element(1, 2),
                                 m.element(2, 3) + m.element(2, 2),
                                 m.element(3, 3) + m.element(3, 2));
        // far
        planes_[plane_far].set(m.element(0, 3) - m.element(0, 2),
                                 m.element(1, 3) - m.element(1, 2),
                                 m.element(2, 3) - m.element(2, 2),
                                 m.element(3, 3) - m.element(3, 2));
    }

    const plane<T>& left_plane() const
    {
        return planes_[plane_left];
    }

    const plane<T>& right_plane() const
    {
        return planes_[plane_right];
    }

    const plane<T>& bottom_plane() const
    {
        return planes_[plane_bottom];
    }

    const plane<T>& top_plane() const
    {
        return planes_[plane_top];
    }

    const plane<T>& near_plane() const
    {
        return planes_[plane_near];
    }

    const plane<T>& far_plane() const
    {
        return planes_[plane_far];
    }

    const plane<T>* planes() const
    {
        return planes_;
    }
private:
    plane<T> planes_[6];
};

template <class T>
bool is_inside(const AABB<T>& aabb, const plane<T>* planes, const plane<T>* abs_planes)
{
    for (size_t i = 0; i < 6; ++i)
    {
        float d = dot(planes[i], aabb.center);
        float r = dot(abs_planes[i], aabb.extents);
        if (d + r < 0)
            return false;
    }
    return true;
}

typedef frustum<float> frustumf;

}

#endif
