#ifndef _RECT_HPP_
#define _RECT_HPP_

#include "vector2d.hpp"
#include <cmath>

namespace mhe
{
    class rect
    {
        private:
            v2d p1, p2;
            float w_, h_;

            void do_cache()
            {
                w_ = fabs(p2.x() - p1.x());
                h_ = fabs(p2.y() - p1.y());
            }
        public:
            rect() {}
            rect(const v2d& ll, const v2d& rh) :
                p1(ll),
                p2(rh)
            {
                do_cache();
            }

            rect(float x, float y, float w, float h) :
                p1(x, y),
                p2(x + w, y + h)
            {
                do_cache();
            }

            float width() const
            {
                return w_;
            }

            float height() const
            {
                return h_;
            }

            bool in(float x, float y) const
            {
                if ( (x >= p1.x()) && (x <= p2.x()) &&
                     (y >= p1.y()) && (y <= p2.y()) )
                    return true;
                return false;
            }

            bool in(const v2d& v) const
            {
                return in(v.x(), v.y());
            }

            rect& operator= (const rect& r)
            {
                p1 = r.p1;
                p2 = r.p2;
                return *this;
            }

            const v2d& ll() const
            {
                return p1;
            }

            const v2d& rh() const
            {
                return p2;
            }
    };
};

#endif
